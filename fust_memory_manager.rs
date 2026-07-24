use std::io::{self, Write};

// Struct untuk mensimulasikan Blok Memori RAM
#[derive(Debug, Clone)]
struct ProcessBlock {
    pid: u32,
    process_name: String,
    ram_usage_mb: u32,
    is_trusted: bool,
}

// Kapasitas RAM Simulasi ZEONOS (misal 512 MB untuk OS minimalis)
const TOTAL_RAM_MB: u32 = 512;

fn print_fust_ram_banner(used_ram: u32) {
    let free_ram = TOTAL_RAM_MB.saturating_sub(used_ram);
    println!(r#"
       \ \___/ /   [FUST MEMORY SENTINEL]
      (  (O) (O) )  "RAM Terpakai: {} MB / {} MB | Sisa: {} MB"
       \   u   /
"#, used_ram, TOTAL_RAM_MB, free_ram);
}

fn main() {
    let mut processes = vec![
        ProcessBlock { pid: 1, process_name: String::from("kernel_core"), ram_usage_mb: 32, is_trusted: true },
        ProcessBlock { pid: 2, process_name: String::from("fust_guard"), ram_usage_mb: 16, is_trusted: true },
        ProcessBlock { pid: 3, process_name: String::from("zeon_shell"), ram_usage_mb: 24, is_trusted: true },
    ];

    loop {
        // Hitung total RAM terpakai saat ini
        let current_ram_used: u32 = processes.iter().map(|p| p.ram_usage_mb).sum();

        print!("{esc}[2J{esc}[1;1H", esc = 27 as char); // Clear screen
        print_fust_ram_banner(current_ram_used);

        println!("=========================================================");
        println!("               ZEONOS MEMORY MANAGER v1.0                ");
        println!("=========================================================");
        println!(" {:<6} | {:<20} | {:<12} | {:<10}", "PID", "Process Name", "RAM Usage", "Status");
        println!("---------------------------------------------------------");

        for p in &processes {
            let status_str = if p.is_trusted {
                "\x1b[32m[TRUSTED]\x1b[0m"
            } else {
                "\x1b[31m[SUSPICIOUS]\x1b[0m"
            };
            println!(" {:<6} | {:<20} | {:<12} | {}", p.pid, p.process_name, format!("{} MB", p.ram_usage_mb), status_str);
        }
        println!("---------------------------------------------------------");

        println!("\n [ OPSIONAL COMMANDS ]");
        println!(" 1. Jalankan Aplikasi Baru (Alokasi RAM)");
        println!(" 2. Kill / Hentikan Proses");
        println!(" 3. Keluar");

        print!("\n Pilih menu [1-3]: ");
        io::stdout().flush().unwrap();

        let mut choice = String::new();
        io::stdin().read_line(&mut choice).unwrap();

        match choice.trim() {
            "1" => {
                print!("\n> Masukkan Nama Proses/Aplikasi: ");
                io::stdout().flush().unwrap();
                let mut app_name = String::new();
                io::stdin().read_line(&mut app_name).unwrap();
                let name = app_name.trim().to_string();

                print!("> Butuh Berapa RAM (MB)?: ");
                io::stdout().flush().unwrap();
                let mut ram_input = String::new();
                io::stdin().read_line(&mut ram_input).unwrap();
                let req_ram: u32 = ram_input.trim().parse().unwrap_or(0);

                if current_ram_used + req_ram > TOTAL_RAM_MB {
                    println!("\n\x1b[31m[FUST BLOCK] GAGAL: RAM Tidak Cukup! Fust mencegah Memory Overflow!\x1b[0m");
                } else {
                    let next_pid = processes.last().map(|p| p.pid + 1).unwrap_or(1);
                    // Cek jika nama mengandung 'hack' atau 'virus'
                    let is_safe = !name.contains("hack") && !name.contains("virus");

                    processes.push(ProcessBlock {
                        pid: next_pid,
                        process_name: name.clone(),
                        ram_usage_mb: req_ram,
                        is_trusted: is_safe,
                    });

                    if is_safe {
                        println!("\n\x1b[32m[SUCCESS] Proses '{}' (PID {}) berhasil dialokasikan!\x1b[0m", name, next_pid);
                    } else {
                        println!("\n\x1b[31m[WARNING] Fust mendeteksi proses mencurigakan '{}'!\x1b[0m", name);
                    }
                }
                print!("\nTekan Enter untuk lanjut...");
                io::stdout().flush().unwrap();
                let _ = io::stdin().read_line(&mut String::new());
            },
            "2" => {
                print!("\n> Masukkan PID yang mau di-Kill: ");
                io::stdout().flush().unwrap();
                let mut pid_input = String::new();
                io::stdin().read_line(&mut pid_input).unwrap();
                let target_pid: u32 = pid_input.trim().parse().unwrap_or(0);

                if target_pid == 1 || target_pid == 2 {
                    println!("\n\x1b[31m[PROTECTED] Fust melarang kamu mematikan Kernel / Fust Guard!\x1b[0m");
                } else {
                    let len_before = processes.len();
                    processes.retain(|p| p.pid != target_pid);
                    if processes.len() < len_before {
                        println!("\n\x1b[32m[SUCCESS] Proses PID {} berhasil dihentikan, RAM dibebaskan!\x1b[0m", target_pid);
                    } else {
                        println!("\n\x1b[31m[ERROR] PID tidak ditemukan!\x1b[0m");
                    }
                }
                print!("\nTekan Enter untuk lanjut...");
                io::stdout().flush().unwrap();
                let _ = io::stdin().read_line(&mut String::new());
            },
            "3" => {
                println!("\nFust Memory Sentinel ditutup. Sistem tetap aman, Captain!");
                break;
            },
            _ => {}
        }
    }
}
