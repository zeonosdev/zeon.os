use std::io::{self, Write};

const OFFICIAL_HASH: &str = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";

struct SystemInfo {
    os_name: String,
    version: String,
    mascot: String,
    kernel_status: String,
}

fn clear_screen() {
    print!("{esc}[2J{esc}[1;1H", esc = 27 as char);
    io::stdout().flush().unwrap();
}

fn print_fust_mini() {
    println!(r#"
       \ \___/ /   [FUST KERNEL SHELL v1.0]
      (  (O) (O) )  "Siap menerima perintah, Captain!"
       \   u   /
"#);
}

fn main() {
    clear_screen();
    
    let sys_info = SystemInfo {
        os_name: String::from("ZEONOS"),
        version: String::from("v1.0-RustCore"),
        mascot: String::from("Fust"),
        kernel_status: String::from("ACTIVE"),
    };

    let local_hash = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";
    let current_user = "captain";

    print_fust_mini();
    println!("=========================================================");
    println!(" Welcome to {} Shell ({})", sys_info.os_name, sys_info.version);
    
    if local_hash == OFFICIAL_HASH {
        println!(" System Integrity: \x1b[32m[OFFICIAL BUILD] 🟢\x1b[0m");
    } else {
        println!(" System Integrity: \x1b[31m[MODIFIED BUILD] 🔴\x1b[0m");
    }
    println!(" Ketik '\x1b[33mhelp\x1b[0m' untuk melihat daftar perintah shell.");
    println!("=========================================================\n");

    loop {
        // Prompt khas ZEONOS
        print!("\x1b[32m{}@zeonos\x1b[0m:\x1b[34m/root\x1b[0m$ ", current_user);
        io::stdout().flush().unwrap();

        let mut input = String::new();
        io::stdin().read_line(&mut input).expect("Gagal membaca input");
        let command = input.trim();

        match command {
            "help" => {
                println!("\n [ DAFTAR PERINTAH ZEONOS SHELL ]");
                println!("  whoami  - Menampilkan user yang sedang login");
                println!("  status  - Cek status sistem & Fust Mascot");
                println!("  ls      - Melihat daftar file sistem /etc/");
                println!("  clear   - Membersihkan layar terminal");
                println!("  exit    - Keluar dari ZEONOS Shell\n");
            },
            "whoami" => {
                println!(" User   : {}", current_user);
                println!(" Role   : OWNER");
                println!(" Mascot : {} (Protecting session)\n", sys_info.mascot);
            },
            "status" => {
                println!("\n [ ZEONOS KERNEL METRICS ]");
                println!("  OS Name   : {}", sys_info.os_name);
                println!("  Version   : {}", sys_info.version);
                println!("  Kernel    : {}", sys_info.kernel_status);
                println!("  Security  : Fust Sentinel Active 👁️\n");
            },
            "ls" => {
                println!("\n [ DIRECTORY: /etc/ ]");
                println!("  - zeon_passwd.json  [USER DATABASE]");
                println!("  - kernel_guard.rs   [SECURITY CORE]");
                println!("  - fust_mascot.art   [MASCOT ASSETS]\n");
            },
            "clear" => {
                clear_screen();
                print_fust_mini();
            },
            "exit" => {
                println!("\n\x1b[33m[FUST]\x1b[0m Shell ditutup. Sampai jumpa lagi, Captain!");
                break;
            },
            "" => continue,
            _ => {
                println!("\x1b[31mzsh: command not found: {}\x1b[0m (Ketik 'help' untuk bantuan)", command);
            }
        }
    }
}
