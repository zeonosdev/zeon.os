/* boot.s - Bootloader Assembly Stage 1 */
.global efi_main
.extern kernel_main

.section .text

# Entry point yang dipanggil oleh Firmware UEFI
efi_main:
    # 1. Simpan parameter UEFI (ImageHandle & SystemTable)
    # Pada ABI x86_64, rcx = ImageHandle, rdx = SystemTable
    pushq %rbp
    movq %rsp, %rbp

    # 2. Panggil fungsi utama C (kernel_main)
    call kernel_main

    # 3. Kembalikan kontrol jika fungsi C selesai
    movq %rbp, %rsp
    popq %rbp
    ret
