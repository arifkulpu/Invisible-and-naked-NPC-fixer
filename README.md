# NPC Fixer - Invisible, Naked, and Headless NPC Fixer (SE / AE / VR)

A modular and high-performance SKSE plugin designed to automatically detect and fix rendering glitches, equipment issues, and visibility bugs for NPCs in **Skyrim Special Edition, Anniversary Edition, and VR**.

---

### [English]

#### Features
- **Invisible NPC & Corpse Fix**: Detects "ghost" NPCs (actors existing in the engine but not rendering) and forces their 3D models to reload. Now also works on **invisible dead bodies (Ghost Corpses)** by using robust `Load3D()` techniques.
- **Naked NPC Fix**: Scans for actors missing body armor/clothing. It checks both the outfit system and the actor's inventory to automatically re-equip missing items.
- **Headless NPC Fix**: Identifies actors with missing head models (FaceGen nodes) and refreshes them.
- **Anti-Flicker Mechanism**: Resets initialization flags and animation states to prevent NPCs from constantly appearing and disappearing.
- **Custom Race Compatibility**: Broadened detection logic to support NPCs from custom race mods. It identifies actors based on playable flags, FaceGen data, and outfit definitions even if they lack standard keywords.
- **Console Command**: Adds `fixnpcs` (or `fnp`) command to manually refresh all nearby actors without waiting for the auto-scan.
- **High Performance & Stability**: Optimized loop running every 5 seconds. Uses a **60-second cooldown per actor** to prevent engine spamming. Includes menu/loading screen protection and a game startup delay to ensure absolute stability. Compatible with `ConsoleUtilSSE`.
- **VR Support**: Fully compatible with Skyrim VR (1.4.15) and the VR Address Library.

#### Requirements
- [SKSE64](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) / [Address Library for SKSE VR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
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
- **Başsız NPC Düzeltmesi**: Kafa modeli (FaceGen) yüklenememiş aktörleri tespit eder ve 3D modellerini tazeler.
- **Flicker (Kaybolma) Engelleme**: Başlangıç bayraklarını ve animasyon sistemini sıfırlayarak NPC'lerin saniyelik olarak görünüp tekrar kaybolmalarını önler.
- **Konsol Komutu**: Otomatik taramayı beklemeden, konsola `fixnpcs` (veya `fnp`) yazarak yakındaki tüm aktörleri anında yenileyebilirsiniz.
- **Yüksek Performans ve Stabilite**: Döngü her 5 saniyede bir çalışır. Motoru yormamak için her aktör başına **60 saniyelik bekleme süresi (cooldown)** uygular. Menüdeyken veya yükleme ekranlarında duraklayarak çökmeleri önler. Yeni oyun/kayıt yükleme başlangıcında 10 sn stabilizasyon gecikmesi eklidir.
- **VR ve Yeni Irk Desteği**: Skyrim VR ile tam uyumludur. Özel ırk modlarıyla eklenen NPC'leri (Custom Races) otomatik olarak algılar ve düzeltir.

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
