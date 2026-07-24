import os
import json

# Hash Resmi Build Asli ZEONOS
OFFICIAL_HASH = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"
PASSWD_FILE = "zeon_passwd.json"

# Color Codes untuk Terminal (Biar Keren!)
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"

# Load / Buat Database User
def load_users():
    if not os.path.exists(PASSWD_FILE):
        default_data = [
            {"uid": 0, "username": "captain", "role": "OWNER", "status": "ACTIVE"}
        ]
        save_users(default_data)
        return default_data
    
    with open(PASSWD_FILE, "r") as f:
        return json.load(f)

def save_users(users):
    with open(PASSWD_FILE, "w") as f:
        json.dump(users, f, indent=4)

def print_dashboard(current_hash, users, current_user):
    os.system('cls' if os.name == 'nt' else 'clear') # Auto Clear Screen
    print("=" * 60)
    print("              ZEONOS CONTROL CENTER                      ")
    
    # Check Integrity Status
    if current_hash == OFFICIAL_HASH:
        print(f"              STATUS: {GREEN}[OFFICIAL BUILD] 🟢{RESET}")
    else:
        print(f"              STATUS: {RED}[WARNING: MODIF BUILD] 🔴{RESET}")
        
    print("=" * 60)
    print(f" Logged in as : {current_user['username']} | Role: {YELLOW}{current_user['role']}{RESET}")
    print("=" * 60)
    
    # Print User Table
    print("\n [ USER MANAGEMENT TABLE ]")
    print("-" * 60)
    print(f" {'UID':<6} | {'Username':<15} | {'Role':<10} | {'Status':<10}")
    print("-" * 60)
    for u in users:
        status_color = RED + f"[{u['status']}]" + RESET if u['status'] == "BANNED" else GREEN + f"[{u['status']}]" + RESET
        print(f" {u['uid']:<6} | {u['username']:<15} | {u['role']:<10} | {status_color}")
    print("-" * 60)

# ==========================================
# MAIN SYSTEM LOGIC
# ==========================================
def main():
    # Ubah string ini jika ingin tes indikator [MODIF BUILD]
    local_hash = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"
    
    users = load_users()
    current_user = users[0] # Default Login: captain (OWNER)

    while True:
        users = load_users()
        print_dashboard(local_hash, users, current_user)
        
        print("\n [ MENU OPERASI OWNER ]")
        print(" 1. Register User Baru")
        print(" 2. Ban / Unban User")
        print(" 3. Ubah Role User (Promote Admin)")
        print(" 4. Keluar Dashboard")
        
        choice = input("\n Pilih Opsi [1-4]: ").strip()

        if choice == "1":
            new_name = input("\n> Masukkan Username Baru: ").strip()
            if any(u['username'] == new_name for u in users):
                print(f"{RED}[ERROR] Username sudah terdaftar!{RESET}")
            else:
                new_uid = 1000 if len(users) == 1 else users[-1]['uid'] + 1
                users.append({"uid": new_uid, "username": new_name, "role": "USER", "status": "ACTIVE"})
                save_users(users)
                print(f"{GREEN}[SUCCESS] User '{new_name}' (UID {new_uid}) berhasil terdaftar!{RESET}")
            input("\nTekan Enter untuk lanjut...")

        elif choice == "2":
            try:
                target_uid = int(input("\n> Masukkan UID user yang mau di-BAN/UNBAN: "))
                user = next((u for u in users if u['uid'] == target_uid), None)
                if user:
                    if user['role'] == "OWNER":
                        print(f"{RED}[ERROR] Akun OWNER tidak bisa di-BAN!{RESET}")
                    else:
                        user['status'] = "BANNED" if user['status'] == "ACTIVE" else "ACTIVE"
                        save_users(users)
                        print(f"{GREEN}[SUCCESS] Status user '{user['username']}' diubah menjadi {user['status']}!{RESET}")
                else:
                    print(f"{RED}[ERROR] UID tidak ditemukan!{RESET}")
            except ValueError:
                print(f"{RED}[ERROR] Input UID harus berupa angka!{RESET}")
            input("\nTekan Enter untuk lanjut...")

        elif choice == "3":
            if current_user['role'] != "OWNER":
                print(f"{RED}[ACCESS DENIED] Cuma OWNER yang bisa ubah role!{RESET}")
            else:
                try:
                    target_uid = int(input("\n> Masukkan UID user yang mau diubah Role: "))
                    user = next((u for u in users if u['uid'] == target_uid), None)
                    if user:
                        if user['role'] == "OWNER":
                            print(f"{RED}[ERROR] Tidak bisa ubah role sesama OWNER!{RESET}")
                        else:
                            print(" Role Pilihan: [1] ADMIN | [2] USER")
                            r_choice = input(" Pilih [1-2]: ").strip()
                            if r_choice == "1":
                                user['role'] = "ADMIN"
                            elif r_choice == "2":
                                user['role'] = "USER"
                            save_users(users)
                            print(f"{GREEN}[SUCCESS] Role '{user['username']}' diubah jadi {user['role']}!{RESET}")
                    else:
                        print(f"{RED}[ERROR] UID tidak ditemukan!{RESET}")
                except ValueError:
                    print(f"{RED}[ERROR] Input UID harus berupa angka!{RESET}")
            input("\nTekan Enter untuk lanjut...")

        elif choice == "4":
            print("\nExiting Dashboard. Sampai jumpa, Captain!")
            break

if __name__ == "__main__":
    main()
