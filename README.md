# 📂 Dropbox

**Dropbox** is a client application with a **SFML-based graphical interface** designed to manage files on a remote server.  

### ✨ Features
- 📤 **Upload files** to the server (including Drag & Drop support).  
- 📥 **Download files** back to your computer.  
- ❌ **Delete** unnecessary files.  
- ⚙️ Configure the server’s **IP address** and **port**.  
- 📁 Choose a local folder to save downloaded files.  
- 🖥️ A simple and user-friendly interface for quick access to all basic functions.  

---

### 🖼️ Screenshot

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://github.com/GiperB0la/Dropbox/blob/main/Screen.jpg">
  <img alt="Screen" src="https://github.com/GiperB0la/Dropbox/blob/main/Screen.jpg">
</picture>

---

## 🛠️ Build

### 🔹 Prerequisites
- **CMake ≥ 3.16**  
- **C++17 compiler** (MSVC, g++, clang++)  
- **SFML 2.6.2** ([download](https://www.sfml-dev.org/download/sfml/2.6.2/))

---

```powershell
# Clone project
git clone https://github.com/GiperB0la/Dropbox.git
cd Dropbox
mkdir build && cd build

# Configure (set SFML_DIR to your SFML cmake folder)
cmake .. -DSFML_DIR="path/to/SFML/lib/cmake/SFML"

# Build (Release version)
cmake --build . --config Release
```
