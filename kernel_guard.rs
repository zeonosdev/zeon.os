use std::io::{self, Write};

// Enum untuk Role User (Gaya khas Rust!)
#[derive(Debug, PartialEq, Clone)]
enum Role {
    Owner,
    Admin,
    User,
}

// Struct Data User
#[derive(Debug, Clone)]
struct ZeonUser {
    uid: u32,
    username: String,
    role: Role,
    is_banned: bool,
}

const OFFICIAL_HASH: &str = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";

fn get_role_str(role: &Role) -> &'static str {
    match role {
        Role::Owner => "OWNER",
        Role::Admin => "ADMIN",
        Role::User  => "USER",
    }
}

fn print_fust_mascot() {
    println!(r#"
       \ \___/ /
      (  (O) (O) )    <-- "FUST IS MONITORING KERNEL MEMORY..."
       \   u   /
        \_____/
      /         \
     (           )
      \_________/
"#);
}

fn main() {
    // Hash lokal untuk tes integritas sistem
    let local_hash = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";

    // Inisialisasi Database User
    let mut users = vec![
        ZeonUser { uid: 0, username: String::from("captain"), role: Role::Owner, is_banned: false },
        ZeonUser { uid: 1000, username: String::from("fust_dev"), role: Role::Admin, is_banned: false },
        ZeonUser { uid: 1001, username: String::from("intruder"), role: Role::User, is_banned: true },
    ];

    print_fust_mascot();
    println!("=========================================================");
    println!("             ZEONOS KERNEL GUARD v2.0                    ");

    // Cek Integritas Build
    if local_hash == OFFICIAL_HASH {
        println!("             STATUS: \x1b[32m[OFFICIAL BUILD] 🟢\x1b[0m");
    } else {
        println!("             STATUS: \x1b[31m[WARNING: MODIF BUILD] 🔴\x1b[0m");
    }
    println!("=========================================================");

    // Loop Utama Login Shell
    loop {
        println!("\n--- [ SIMULASI LOGIN SHELL ZEONOS ] ---");
        print!("zeonos login: ");
        io::stdout().flush().unwrap();

        let mut input_user = String::new();
        io::stdin().read_line(&mut input_user).expect("Gagal membaca input");
        let username_input = input_user.trim();

        if username_input == "exit" {
            println!("\n[GUARD] Shutting down ZEONOS Kernel Shell...");
            break;
        }

        // Cari user di database memori
        let found_user = users.iter_mut().find(|u| u.username == username_input);

        match found_user {
            Some(user) => {
                if user.is_banned {
                    println!("\x1b[31m[ACCESS DENIED] User '{}' sedang di-BAN oleh Fust!\x1b[0m", user.username);
                } else {
                    println!("\x1b[32m[SUCCESS] Selamat datang, {}! (Role: {})\x1b[0m", user.username, get_role_str(&user.role));
                    
                    // Fitur Khusus OWNER
                    if user.role == Role::Owner {
                        println!("\n[OWNER MENU] Ketik 'ban' untuk nge-BAN user 'intruder' atau 'enter' untuk skip: ");
                        let mut opt = String::new();
                        io::stdin().read_line(&mut opt).unwrap();
                        
                        if opt.trim() == "ban" {
                            if let Some(target) = users.iter_mut().find(|u| u.username == "intruder") {
                                target.is_banned = true;
                                println!("\x1b[33m[KERNEL ACTION] User 'intruder' berhasil di-BAN permanen!\x1b[0m");
                            }
                        }
                    }
                }
            },
            None => {
                println!("\x1b[31m[ERROR] User '{}' tidak terdaftar di VFS /etc/passwd!\x1b[0m", username_input);
            }
        }
    }
}
