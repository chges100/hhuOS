; Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
; Heinrich-Heine University; Olaf Spinczyk, TU Dortmund
;
; This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
; License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
; later version.
;
; This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
; warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
; details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>

global startFirstThread
global switchContext

extern releaseSchedulerLock
extern flush_tss

section .text

startFirstThread:

    ; get the thread's context
    mov	esp, [esp + 0x04]

    ; load registers
    pop edi
    pop esi
    pop ebx
    pop ebp

    call flush_tss

    call releaseSchedulerLock

    ; start thread
    ret

switchContext:

    ; get both thread's contexts
    mov eax, [esp + 0x04]
    mov edx, [esp + 0x08]

    ; save current thread's context
    push ebp
    push ebx
    push esi
    push edi

    ; switch stacks
    mov [eax], esp
    mov esp, [edx]

    ; load next thread's context
    pop edi
    pop esi
    pop ebx
    pop ebp

    call releaseSchedulerLock

    ; resume next thread
    ret
