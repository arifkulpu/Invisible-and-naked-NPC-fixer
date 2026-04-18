#include "PCH.h"
#include "VisibilityFixer.h"
#include "Settings.h"

#include <unordered_set>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

namespace VisibilityFixer
{
    // Bekleyen düzeltme işlemleri için yapı (Performans için kademeli işleme)
    struct PendingFix
    {
        RE::ObjectRefHandle actorHandle;
        std::chrono::steady_clock::time_point fixTime;
    };

    // Aktör bazlı son fix zamanları (Performansı korumak ama hatalı fixleri tekrar denemek için 60sn cooldown)
    std::unordered_map<RE::FormID, std::chrono::steady_clock::time_point> actorFixTimestamps;
    
    // İşlem kuyruğu (Spam engelleme ve performans için)
    std::vector<PendingFix> pendingFixQueue;

    // Oyun başlangıç zamanı (Yeni oyun/yükleme sonrası stabilizasyon için)
    std::chrono::steady_clock::time_point systemStartTime = std::chrono::steady_clock::now();

    void ClearFixedActors()
    {
        actorFixTimestamps.clear();
        pendingFixQueue.clear();
        systemStartTime = std::chrono::steady_clock::now(); // Zamanlayıcıyı sıfırla
        logger::info("VisibilityFixer: Fix listesi ve baslangic zamanlayicisi sifirlandi.");
    }

    bool IsHumanoidNPC(RE::Actor* a_actor)
    {
        if (!a_actor) return false;
        
        auto race = a_actor->GetRace();
        if (!race) return false;

        // 1. Anahtar kelime kontrolü
        if (race->HasKeywordString("ActorTypeNPC") || race->HasKeywordString("ActorTypeHumanoid")) {
            return true;
        }

        // 2. Irk bayrakları kontrolü (Oynanabilir veya FaceGen verisi olanlar insansıdır)
        if (race->data.flags.any(RE::RACE_DATA::Flag::kPlayable, RE::RACE_DATA::Flag::kFaceGenHead)) {
            return true;
        }

        // 3. Kıyafet tanımlı mı? (Yeni ırk modları için en sağlam yöntem)
        auto npcBase = a_actor->GetActorBase();
        if (npcBase && (npcBase->defaultOutfit || npcBase->sleepOutfit)) {
            return true;
        }

        return false;
    }

    void ProcessActorFix(RE::Actor* actor)
    {
        if (!actor || actor->IsDeleted() || !actor->Is3DLoaded()) return;

        auto root = actor->Get3D();
        auto formID = actor->GetFormID();
        
        // Aktörün geçerli bir form tipine sahip olduğundan emin ol
        if (actor->GetFormType() != RE::FormType::ActorCharacter) return;

        logger::info("[Düzeltme] {} ({:08X}) analizi basladi...", actor->GetName(), formID);

        bool fixed = false;

        // 1. Görünmezlik (Ghost / Görünmez Ceset) Düzeltmesi
        if (!root) {
            if (actor->IsDead()) {
                logger::info(" -> Görünmez Ceset (Ghost Corpse) tespit edildi. Load3D zorlanıyor...");
            } else {
                logger::info(" -> Görünmezlik (Ghost) tespit edildi. Load3D zorlanıyor...");
            }
            
            // kDisabled ve kInitiallyDisabled flag'lerini temizle (Flicker engelleme)
            actor->formFlags &= ~RE::TESForm::RecordFlags::kDisabled;
            actor->formFlags &= ~RE::TESObjectREFR::RecordFlags::kInitiallyDisabled;
            
            // Motoru modeli yeniden yüklemeye zorla
            actor->Load3D(false);
            
            fixed = true;
        } 
        // 2. Çıplaklık ve Kıyafet Düzeltmesi
        else {
            if (IsHumanoidNPC(actor)) {
                // Vücut zırhı var mı? (currentProcess kontrolü ile güvenli çağrı)
                RE::TESObjectARMO* chestArmor = nullptr;
                if (actor->GetActorRuntimeData().currentProcess) {
                    chestArmor = actor->GetWornArmor(RE::BIPED_MODEL::BipedObjectSlot::kBody);
                }

                if (!chestArmor) {
                    logger::info(" -> Çıplaklık tespit edildi. Envanter taranıyor...");
                    
                    // Envanterde vücut zırhı var mı bak
                    auto inv = actor->GetInventory();
                    for (auto& [item, data] : inv) {
                        if (item && item->IsArmor()) {
                            auto armor = item->As<RE::TESObjectARMO>();
                            if (armor && armor->HasPartOf(RE::BIPED_MODEL::BipedObjectSlot::kBody)) {
                                logger::info(" -> Envanterde zırh bulundu: {}. Giydiriliyor...", armor->GetName());
                                RE::ActorEquipManager::GetSingleton()->EquipObject(actor, armor, nullptr, 1, nullptr, true, false, false, false);
                                fixed = true;
                                break;
                            }
                        }
                    }

                    // Envanterde yoksa Outfit'i sıfırla
                    if (!fixed) {
                        auto npcBase = actor->GetActorBase();
                        if (npcBase && (npcBase->defaultOutfit || npcBase->sleepOutfit)) {
                            logger::info(" -> Outfit tanımlı. Envanter sıfırlanarak kıyafetler zorlanıyor...");
                            actor->ResetInventory(false);
                            fixed = true;
                        }
                    }
                }

                // Başsızlık kontrolü
                if (!root->GetObjectByName("FaceGenNiNode")) {
                    logger::info(" -> Başsızlık düzeltiliyor...");
                    fixed = true;
                }
            }
        }

        if (fixed) {
            actor->Update3DModel();
            actor->UpdateAnimation(0.0f); // Animasyon sistemini tetikleyerek görünürlüğü garanti altına al
            logger::info(" -> Düzeltme işlemi tamamlandı.");
        } else {
            logger::info(" -> Düzeltme gerekmedi veya yapılamadı.");
        }
    }

    void FixActor(RE::Actor* actor, std::chrono::steady_clock::time_point now, bool a_force)
    {
        if (!actor || actor->IsPlayerRef() || actor->IsDeleted()) return;
        if (actor->IsDisabled()) return; 
        
        // 3D yüklü değilse veya aktör geçerli değilse işlem yapma
        if (!actor->Is3DLoaded() || actor->GetFormType() != RE::FormType::ActorCharacter) return;

        auto formID = actor->GetFormID();

        // Bekleme süresi kontrolü (INI'den alınan değer)
        auto it = actorFixTimestamps.find(formID);
        if (!a_force && it != actorFixTimestamps.end()) {
            if (now - it->second < std::chrono::seconds(Settings::GetSingleton().cooldown)) {
                return;
            }
        }

        bool needsFix = false;

        // --- GÖRÜNMEZLİK (GHOST) KONTROLÜ ---
        if (actor->Get3D() == nullptr) {
            logger::info("[Görünmezlik] {} ({:08X}) tespit edildi.", actor->GetName(), formID);
            needsFix = true;
        }

        // --- ÇIPLAKLIK VE BAŞSIKLIK KONTROLLERİ ---
        if (!needsFix) {
            auto root = actor->Get3D();
            if (root) {
                // Başsızlık kontrolü
                if (!root->GetObjectByName("FaceGenNiNode")) {
                    logger::info("[Başsızlık] {} ({:08X}) tespit edildi.", actor->GetName(), formID);
                    needsFix = true;
                }
                
                // Çıplaklık kontrolü (NPC ise)
                if (!needsFix) {
                    if (IsHumanoidNPC(actor)) {
                        RE::TESObjectARMO* chestArmor = nullptr;
                        if (actor->GetActorRuntimeData().currentProcess) {
                            chestArmor = actor->GetWornArmor(RE::BIPED_MODEL::BipedObjectSlot::kBody);
                        }
                        
                        if (!chestArmor) {
                            logger::info("[Çıplaklık] {} ({:08X}) tespit edildi.", actor->GetName(), formID);
                            needsFix = true;
                        }
                    }
                }
            }
        }

        if (needsFix) {
            // Kuyruğa ekle (Aynı frame içinde spam engelleme ve 0.2s bekleme kuralı)
            PendingFix pending;
            pending.actorHandle = actor->GetHandle();
            pending.fixTime = now + 200ms;
            
            pendingFixQueue.push_back(pending);
            actorFixTimestamps[formID] = now; // Fix zamanını kaydet
            
            if (Settings::GetSingleton().logFixes) {
                logger::info(" -> Aktör kuyruğa eklendi ({:08X}), 0.2s sonra düzeltilecek.", formID);
            }
        }
    }

    void ProcessFixes(bool a_force)
    {
        // Menü veya yükleme ekranındaysak işlem yapma (Cell load koruması)
        auto ui = RE::UI::GetSingleton();
        if (ui && ui->GameIsPaused()) {
            return;
        }

        auto processLists = RE::ProcessLists::GetSingleton();
        if (!processLists) return;

        auto now = std::chrono::steady_clock::now();

        for (auto& handle : processLists->highActorHandles) {
            auto actor = handle.get();
            if (actor) {
                FixActor(actor.get(), now, a_force);
            }
        }
    }

    void ProcessQueue(std::chrono::steady_clock::time_point now)
    {
        if (pendingFixQueue.empty()) return;

        for (auto it = pendingFixQueue.begin(); it != pendingFixQueue.end(); ) {
            auto ref = it->actorHandle.get();
            if (ref && now >= it->fixTime) {
                auto actor = ref->As<RE::Actor>();
                if (actor) {
                    ProcessActorFix(actor);
                }
                it = pendingFixQueue.erase(it);
            } else if (!ref) {
                it = pendingFixQueue.erase(it);
            } else {
                ++it;
            }
        }
    }

    void Update()
    {
        auto now = std::chrono::steady_clock::now();
        
        // Stabilizasyon için baslangic gecikmesi (Settings üzerinden)
        if (now - systemStartTime < std::chrono::seconds(Settings::GetSingleton().startupDelay)) {
            return;
        }

        // Kuyruğu her karede işle
        ProcessQueue(now);

        static auto lastUpdate = now;
        // Taramalar arası bekleme (Settings üzerinden)
        if (now - lastUpdate > std::chrono::seconds(Settings::GetSingleton().scanInterval)) {
            ProcessFixes(false);
            lastUpdate = now;
        }
    }

    // Ana döngü kancası
    struct MainUpdateHook
    {
        static void thunk(RE::Main* a_this, float a_delta)
        {
            func(a_this, a_delta);
            Update();
        }
        static inline REL::Relocation<decltype(thunk)> func;
    };

    void Install()
    {
        // Main::Update (Skyrim SE 1.5.97 için ID 35551, Offset 0x11F | Skyrim AE 1.6+ için ID 36544, Offset 0x160 | Skyrim VR için Offset 0x11F)
        REL::Relocation<std::uintptr_t> target{ REL::RelocationID(35551, 36544), static_cast<std::ptrdiff_t>(REL::VariantOffset(0x11F, 0x160, 0x11F).offset()) }; 
        
        SKSE::AllocTrampoline(14);
        MainUpdateHook::func = SKSE::GetTrampoline().write_call<5>(target.address(), MainUpdateHook::thunk);
        
        logger::info("VisibilityFixer: Sistem basariyla yuklendi.");
    }
}
