import os
import json

OFFICIAL_HASH = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"
PASSWD_FILE = "zeon_passwd.json"

GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"

# ASCII ART FUST
FUST_ART = f"""
       {YELLOW}/ \\___/ \\{RESET}
      {YELLOW}(  (O) (O) ){RESET}    {GREEN}<-- "FUST IS WATCHING THE KERNEL..."{RESET}
       {YELLOW}\\   {RED}u{RESET}{YELLOW}   /{RESET}
        {YELLOW}\\_____/{RESET}
      {YELLOW}/         \\{RESET}
     {YELLOW}(           ){RESET}
      {YELLOW}\\_________/{RESET}
"""

def load_users():
    if not os.path.exists(PASSWD_FILE):
        default_data = [{"uid": 0, "username": "captain", "role": "OWNER", "status": "ACTIVE"}]
        save_users(default_data)
        return default_data
    with open(PASSWD_FILE, "r") as f:
        return json.load(f)

def save_users(users):
    with open(PASSWD_FILE, "w") as f:
        json.dump(users, f, indent=4)

def print_dashboard(current_hash, users, current_user):
    os.system('cls' if os.name == 'nt' else 'clear')
    print(FUST_ART)
    print("=" * 60)
    print("              ZEONOS CONTROL CENTER                      ")
    
    if current_hash == OFFICIAL_HASH:
        print(f"              STATUS: {GREEN}[OFFICIAL BUILD] 🟢{RESET}")
    else:
        print(f"              STATUS: {RED}[WARNING: MODIF BUILD] 🔴{RESET}")
        
    print("=" * 60)
    print(f" Logged in as : {current_user['username']} | Role: {YELLOW}{current_user['role']}{RESET}")
    print("=" * 60)
    
    print("\n [ USER MANAGEMENT TABLE ]")
    print("-" * 60)
    print(f" {'UID':<6} | {'Username':<15} | {'Role':<10} | {'Status':<10}")
    print("-" * 60)
    for u in users:
        status_color = RED + f"[{u['status']}]" + RESET if u['status'] == "BANNED" else GREEN + f"[{u['status']}]" + RESET
        print(f" {u['uid']:<6} | {u['username']:<15} | {u['role']:<10} | {status_color}")
    print("-" * 60)

def main():
    local_hash = "ZEONOS_v1.0_OFFICIAL_SECURE_KEY_2026"
    users = load_users()
    current_user = users[0]

    while True:
        users = load_users()
        print_dashboard(local_hash, users, current_user)
        
        print("\n [ MENU OPERASI OWNER ]")
        print(" 1. Register User Baru")
        print(" 2. Ban / Unban User")
        print(" 3. Keluar Dashboard")
        
        choice = input("\n Pilih Opsi [1-3]: ").strip()

        if choice == "1":
            new_name = input("\n> Masukkan Username Baru: ").strip()
            if any(u['username'] == new_name for u in users):
                print(f"{RED}[ERROR] Username sudah ada!{RESET}")
            else:
                new_uid = 1000 if len(users) == 1 else users[-1]['uid'] + 1
                users.append({"uid": new_uid, "username": new_name, "role": "USER", "status": "ACTIVE"})
                save_users(users)
                print(f"{GREEN}[SUCCESS] User '{new_name}' terdaftar!{RESET}")
            input("\nTekan Enter...")

        elif choice == "2":
            try:
                target_uid = int(input("\n> Masukkan UID user: "))
                user = next((u for u in users if u['uid'] == target_uid), None)
                if user:
                    if user['role'] == "OWNER":
                        print(f"{RED}[ERROR] OWNER tidak bisa di-BAN!{RESET}")
                    else:
                        user['status'] = "BANNED" if user['status'] == "ACTIVE" else "ACTIVE"
                        save_users(users)
                        print(f"{GREEN}[SUCCESS] Status diubah!{RESET}")
                else:
                    print(f"{RED}[ERROR] UID tidak ditemukan!{RESET}")
            except ValueError:
                print(f"{RED}[ERROR] Masukkan angka!{RESET}")
            input("\nTekan Enter...")

        elif choice == "3":
            print("\nFust Pamit. Sampai Jumpa, Captain!")
            break

if __name__ == "__main__":
    main()
