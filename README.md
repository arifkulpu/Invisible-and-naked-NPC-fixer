# NPC Fixer - Invisible, Naked, and Headless NPC Fixer (SE / AE / VR)

A modular and high-performance SKSE plugin designed to automatically detect and fix rendering glitches, equipment issues, and visibility bugs for NPCs in **Skyrim Special Edition, Anniversary Edition, and VR**.

---

### [English]

#### Features
- **Invisible NPC & Corpse Fix**: Detects "ghost" NPCs (actors existing in the engine but not rendering) and forces their 3D models to reload. Now also works on **invisible dead bodies (Ghost Corpses)** by using robust `Load3D()` techniques.
- **Naked NPC Fix**: Scans for actors missing body armor/clothing. It checks both the outfit system and the actor's inventory to automatically re-equip missing items.
- **Headless NPC Fix**: Identifies actors with missing head models (FaceGen nodes) and refreshes them.
- **Anti-Flicker Mechanism**: Resets initialization flags and animation states to prevent NPCs from constantly appearing and disappearing.
- **Event-Driven Architecture**: Migrated from a heavy polling loop to a high-performance event-based system. Uses `TESObjectLoadedEvent` and `TESContainerChangedEvent` to trigger fixes only when necessary, significantly reducing CPU overhead.
- **SexLab & OStim Integration**: Robust scene-aware protection. Prevents the plugin from interfering with NPCs during SexLab stages or OStim animations to avoid scene breaking or premature re-equipping.
- **Gameplay Protection Logic**: Automatically detects and protects actors currently in **Dialogue, Combat, or Quest** states to ensure immersion and quest stability.
- **Spam & Race Protection**: Includes a 0.5s safety delay and queue system to prevent equipment spam. Supports custom race mods and uses actor-specific cooldowns.
- **Follower Mod Compatibility**: Added robust protection for follower frameworks like **AFT, NFF, EFF, and Simple Outfit System**. The plugin automatically ignores actors marked as `IsPlayerTeammate` or in common follower factions to prevent conflicts. Destructive inventory resets are disabled for followers to prevent equipment loss.
- **Console Command**: Adds `fixnpcs` (or `fnp`) command to manually refresh all nearby actors without waiting for the auto-scan.
- **VR Support**: Fully compatible with Skyrim VR (1.4.15) and the VR Address Library.

#### Requirements
- [SKSE64](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) / [Address Library for SKSE VR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
- [Microsoft Visual C++ Redistributable 2019-2022](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
- Skyrim SE (1.5.97), AE (1.6+), or VR (1.4.15)

#### Installation
1. Download the `NakedNPCFix.dll`.
2. Place it in `Data/SKSE/Plugins/`.
3. (Optional) Create `Data/SKSE/Plugins/NakedNPCFix.ini` to customize settings.
4. Check `Documents/My Games/Skyrim [Version]/SKSE/NPCFixer.log` for logs.

---

### [Türkçe]

#### Özellikler
- **Görünmez NPC ve Ceset Düzeltmesi**: Motor üzerinde var olan ancak görseli yüklenmeyen (hayalet) NPC'leri ve **görünmez cesetleri (Ghost Corpses)** tespit eder. `Load3D()` kullanarak modelleri zorla geri getirir.
- **Çıplak NPC Düzeltmesi**: Vücut zırhı veya kıyafeti eksik olan NPC'leri tarar. Hem kıyafet seti (outfit) sistemini hem de NPC'nin envanterini kontrol ederek eksik eşyaları otomatik giydirir.
- **Olay Bazlı (Event-Driven) Sistem**: Sürekli tarama yapan ağır döngü yerine `TESObjectLoadedEvent` ve `TESContainerChangedEvent` kullanan modern sistem. Sadece gerekli anlarda tetiklenerek CPU kullanımını minimize eder.
- **SexLab ve OStim Entegrasyonu**: Aktif sahneleri algılar. Animasyon sırasında NPC'lerin kıyafetlerinin motor tarafından zorla giydirilmesini veya sahnenin bozulmasını engeller.
- **Gelişmiş Koruma Mantığı**: **Diyalog, Savaş veya Quest (Görev)** aşamasındaki aktörleri otomatik korumaya alır. Bu sayede görevlerin bozulması veya daldırma (immersion) kaybı önlenir.
- **Kuyruk ve Spam Koruması**: İşlemler arasına 0.5 saniyelik güvenlik gecikmesi ve her aktöre özel bekleme süresi (cooldown) eklenerek motorun yorulması engellenir.
- **Konsol Komutu**: Otomatik taramayı beklemeden, konsola `fixnpcs` (veya `fnp`) yazarak yakındaki tüm aktörleri anında yenileyebilirsiniz.
- **VR ve Özel Irk Desteği**: Skyrim VR ile tam uyumludur. Özel ırk modlarıyla (Custom Races) eklenen NPC'leri algılar ve düzeltir.
- **Takipçi Modu Uyumluluğu**: **AFT, NFF, EFF ve Simple Outfit System** gibi takipçi sistemleriyle tam uyum. `IsPlayerTeammate` bayrağına sahip veya takipçi faction'larındaki aktörler otomatik olarak yoksayılır. Takipçilerin özel kıyafet sistemlerini bozmamak için bu aktörlerde "Envanter Sıfırlama" işlemi devre dışı bırakılmıştır.

#### Gereksinimler
- [SKSE64](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
- [Microsoft Visual C++ Redistributable 2019-2022](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)

#### Kurulum
1. `NakedNPCFix.dll` dosyasını indirin.
2. `Data/SKSE/Plugins/` klasörüne kopyalayın.
3. `Documents/My Games/Skyrim Special Edition/SKSE/NPCFixer.log` dosyasından durumu takip edebilirsiniz.

---

## Build (Geliştiriciler İçin)
This project uses **vcpkg** and **CMake**.
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

## Credits
- Developed by Antigravity (Pair Programming with arifkulpu)
- Uses [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG)

## License / Lisans
Copyright (c) 2026 Arif KULPU. All Rights Reserved. — Tüm Hakları Saklıdır.
See LICENSE for details.
