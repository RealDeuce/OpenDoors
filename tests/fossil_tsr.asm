; Minimal FOSSIL-compatible test fixture for the DOS32 CI tests.

.8086
.model tiny
.code
org 100h

start:
        mov     dx, offset fossil_handler
        mov     ax, 2514h
        int     21h
        mov     dx, offset resident_end
        add     dx, 15
        mov     cl, 4
        shr     dx, cl
        mov     ax, 3100h
        int     21h

fossil_handler proc far
        cmp     ah, 00h
        je      simple_success
        cmp     ah, 02h
        je      receive_byte
        cmp     ah, 03h
        je      status
        cmp     ah, 04h
        je      detect
        cmp     ah, 05h
        je      simple_success
        cmp     ah, 06h
        je      set_dtr
        cmp     ah, 09h
        je      simple_success
        cmp     ah, 0ah
        je      clear_input
        cmp     ah, 0bh
        je      send_byte
        cmp     ah, 0fh
        je      simple_success
        cmp     ah, 18h
        je      receive_block
        cmp     ah, 19h
        je      send_block
        xor     ax, ax
        iret

detect:
        mov     ax, 1954h
        iret

simple_success:
        mov     ax, 1
        iret

set_dtr:
        mov     cs:dtr_state, al
        mov     ax, 1
        iret

clear_input:
        mov     word ptr cs:rx_head, 0
        mov     word ptr cs:rx_tail, 0
        mov     word ptr cs:rx_count, 0
        mov     ax, 1
        iret

status:
        mov     ax, 4080h
        cmp     word ptr cs:rx_count, 0
        je      status_done
        or      ah, 01h
status_done:
        iret

send_byte:
        push    bx
        mov     bx, cs:rx_tail
        mov     cs:rx_buffer[bx], al
        inc     bx
        and     bx, 0fffh
        mov     cs:rx_tail, bx
        cmp     word ptr cs:rx_count, 4096
        jae     send_byte_done
        inc     word ptr cs:rx_count
send_byte_done:
        pop     bx
        mov     ax, 1
        iret

receive_byte:
        push    bx
        xor     ax, ax
        cmp     word ptr cs:rx_count, 0
        je      receive_byte_done
        mov     bx, cs:rx_head
        mov     al, cs:rx_buffer[bx]
        inc     bx
        and     bx, 0fffh
        mov     cs:rx_head, bx
        dec     word ptr cs:rx_count
receive_byte_done:
        pop     bx
        iret

send_block:
        push    bx
        push    cx
        push    di
        push    si
        xor     si, si
send_block_loop:
        cmp     cx, 0
        je      send_block_done
        cmp     word ptr cs:rx_count, 4096
        jae     send_block_done
        mov     bx, cs:rx_tail
        mov     al, es:[di]
        mov     cs:rx_buffer[bx], al
        inc     di
        inc     bx
        and     bx, 0fffh
        mov     cs:rx_tail, bx
        inc     word ptr cs:rx_count
        inc     si
        dec     cx
        jmp     send_block_loop
send_block_done:
        mov     ax, si
        pop     si
        pop     di
        pop     cx
        pop     bx
        iret

receive_block:
        push    bx
        push    cx
        push    di
        push    si
        xor     si, si
receive_block_loop:
        cmp     cx, 0
        je      receive_block_done
        cmp     word ptr cs:rx_count, 0
        je      receive_block_done
        mov     bx, cs:rx_head
        mov     al, cs:rx_buffer[bx]
        mov     es:[di], al
        inc     di
        inc     bx
        and     bx, 0fffh
        mov     cs:rx_head, bx
        dec     word ptr cs:rx_count
        inc     si
        dec     cx
        jmp     receive_block_loop
receive_block_done:
        mov     ax, si
        pop     si
        pop     di
        pop     cx
        pop     bx
        iret

fossil_handler endp

dtr_state db 1
rx_head   dw 0
rx_tail   dw 0
rx_count  dw 0
rx_buffer db 4096 dup (0)

resident_end label byte

end start
