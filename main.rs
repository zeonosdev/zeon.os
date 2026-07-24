use std::io::{self, Write};

// Define struct User
struct ZeonUser {
    uid: u32,
    username: String,
    role: String,
    is_banned: bool,
}

const OFFICIAL_HASH: &str = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";

fn main() {
    let local_hash = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026";

    // Membuat list user (Vector di Rust)
    let mut users = vec![
        ZeonUser { uid: 0, username: String::from("captain"), role: String::from("OWNER"), is_banned: false },
        ZeonUser { uid: 1001, username: String::from("penipu_x"), role: String::from("USER"), is_banned: false },
    ];

    println!("=========================================================");
    println!("              ZEONOS CONTROL CENTER (RUST)               ");
    
    // Cek Hash System (Integrity Check)
    if local_hash == OFFICIAL_HASH {
        println!("              STATUS: \x1b[32m[OFFICIAL BUILD] 🟢\x1b[0m");
    } else {
        println!("              STATUS: \x1b[31m[WARNING: MODIF BUILD] 🔴\x1b[0m");
    }
    println!("=========================================================");

    // Tampilkan List User
    println!("\n[ DAFTAR USER ZEONOS ]");
    for u in &users {
        let status = if u.is_banned { "\x1b[31m[BANNED]\x1b[0m" } else { "\x1b[32m[ACTIVE]\x1b[0m" };
        println!("UID: {:<5} | Username: {:<10} | Role: {:<6} | Status: {}", u.uid, u.username, u.role, status);
    }

    // Input UID untuk Ban
    print!("\n> Masukkan UID user yang mau di-BAN: ");
    io::stdout().flush().unwrap();

    let mut input = String::new();
    io::stdin().read_line(&mut input).expect("Gagal membaca input");
    let target_uid: u32 = input.trim().parse().unwrap_or(9999);

    // Cari dan BAN User
    let mut found = false;
    for u in &mut users {
        if u.uid == target_uid {
            found = true;
            if u.role == "OWNER" {
                println!("\x1b[31m[ERROR] GAGAL: Akun OWNER tidak bisa di-ban!\x1b[0m");
            } else {
                u.is_banned = true;
                println!("\x1b[32m[SUCCESS] User '{}' berhasil DI-BAN oleh Rust Kernel!\x1b[0m", u.username);
            }
            break;
        }
    }

    if !found {
        println!("\x1b[31m[ERROR] UID tidak ditemukan!\x1b[0m");
    }
}
