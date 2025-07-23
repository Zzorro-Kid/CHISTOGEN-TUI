# 🧹 CHISTOGEN - Advanced System Cleaner Utility

A powerful Linux system cleaning tool with intelligent analysis and secure deletion capabilities.

<img width="587" height="132" alt="image" src="https://github.com/user-attachments/assets/c0c1ea52-ea40-457d-bf45-cef8d206edd8" />


## 📦 Features

  - Comprehensive junk file cleaning (`/tmp`, `~/.cache`, package manager caches)
  - Interactive disk usage visualization
  - Smart duplicate file detection
  - Military-grade secure file deletion
  - Automatic log rotation and cleanup
  - Personal data scanner (CC numbers, emails, etc.)
  - Performance optimization tools
 
 
## 🔧 Prerequisites

  - Linux operating system.
  - Makefile installed.
  - GCC compiler to build the project.
  - At least one terminal emulator (default).
 
 
## 🚀 Running

   1. Clone or download this repository:
 
    git clone https://github.com/Zzorro-Kid/CHISTOGEN-TUI.git

   2. Navigate to the project directory:

    cd CHISTOGEN
    
   3. Build project using **Makefile** (or if you don't have it, install firstly):

    sudo make install
    make

   4. Install to system binaries:

    sudo cp chistogen /usr/local/bin/chistogen
    sudo chmod +x /usr/local/bin/chistogen
    
   5. Then, alias setup, add to your shell config (**~/.bashrc** or **~/.zshrc**) :

    echo 'alias chistogen="sudo /usr/local/bin/chistogen"' >> ~/.bashrc // or (~/.zshrc)
    source ~/.bashrc // or (~/.zshrc)
    
   6. Make it executable:
   
    sudo chmod +x /usr/local/bin/chistogen
    
   7. Finally, run it by printing in console:
   
    chistogen

