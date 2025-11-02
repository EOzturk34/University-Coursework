#!/usr/bin/env python3
import sys
import argparse

class GTUC312CPU:
    def __init__(self):
        # 20000 bellek konumu
        self.memory = [0] * 20000
        self.instructions = {}
        self.halted = False
        self.kernel_mode = True
        self.current_thread = 0
        self.blocked_until = -1  # PRN system call için
        
        # Stack pointer'ı en yüksek adresten başlat
        self.memory[1] = 19999
        
    def load_program(self, filename):
        """Program dosyasını yükle"""
        with open(filename, 'r') as f:
            lines = f.readlines()
        
        in_data_section = False
        in_instruction_section = False
        has_user_instruction = False
        has_os_data = False
        
        for line in lines:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
                
            if line == "Begin Data Section":
                in_data_section = True
                continue
            elif line == "End Data Section":
                in_data_section = False
                continue
            elif line == "Begin Instruction Section":
                in_instruction_section = True
                continue
            elif line == "End Instruction Section":
                in_instruction_section = False
                continue
                
            if in_data_section:
                parts = line.split()
                if len(parts) >= 2:
                    addr = int(parts[0])
                    value = int(parts[1])
                    self.memory[addr] = value
                    # Check if this looks like OS data (addresses 21-999)
                    if 21 <= addr <= 999:
                        has_os_data = True
                    
            elif in_instruction_section:
                # Komut satırından # sonrasını temizle
                if '#' in line:
                    line = line[:line.index('#')].strip()
                if line:
                    # İlk sayıyı (instruction address) al, geri kalanını instruction olarak sakla
                    parts = line.split()
                    if len(parts) >= 2:
                        instr_addr = int(parts[0])
                        instr_text = ' '.join(parts[1:])
                        self.instructions[instr_addr] = instr_text
                        # Check if this program has USER instruction (OS program)
                        if parts[1].upper() == "USER":
                            has_user_instruction = True
        
        # Determine initial mode based on program type
        if has_user_instruction or has_os_data:
            # This is an OS program - start in kernel mode
            self.kernel_mode = True
        else:
            # This is a simple user program - start in user mode
            self.kernel_mode = False
    
    def execute(self):
        """Tek bir instruction çalıştır"""
        if self.halted:
            return False
            
        # PRN bloklama kontrolü
        if self.blocked_until > 0:
            if self.memory[3] < self.blocked_until:
                # Bloklama sırasında sadece instruction count artır
                self.memory[3] += 1
                return True
            else:
                self.blocked_until = -1
                
        pc = self.memory[0]  # Program Counter
        
        if pc not in self.instructions:
            self.halted = True
            return False
            
        instruction = self.instructions[pc].strip()
        parts = instruction.split()
        
        if not parts:
            self.memory[0] += 1
            return True
            
        cmd = parts[0].upper()
        
        # Instruction sayısını artır
        self.memory[3] += 1
        
        try:
            if cmd == "SET":
                # SET B A - Set value B into address A
                value = int(parts[1])
                addr = int(parts[2])
                self.check_memory_access(addr)
                self.memory[addr] = value
                
            elif cmd == "CPY":
                # CPY A1 A2 - Copy content of A1 to A2
                addr1 = int(parts[1])
                addr2 = int(parts[2])
                self.check_memory_access(addr1)
                self.check_memory_access(addr2)
                self.memory[addr2] = self.memory[addr1]
                
            elif cmd == "CPYI":
                # CPYI A1 A2 - Indirect copy: copy content of address indexed by A1 to A2
                addr1 = int(parts[1])
                addr2 = int(parts[2])
                self.check_memory_access(addr1)
                self.check_memory_access(addr2)
                indirect_addr = self.memory[addr1]
                self.check_memory_access(indirect_addr)
                self.memory[addr2] = self.memory[indirect_addr]
                
            elif cmd == "CPYI2":
                # CPYI2 A1 A2 - Double indirect copy
                addr1 = int(parts[1])
                addr2 = int(parts[2])
                self.check_memory_access(addr1)
                self.check_memory_access(addr2)
                indirect_addr1 = self.memory[addr1]
                indirect_addr2 = self.memory[addr2]
                self.check_memory_access(indirect_addr1)
                self.check_memory_access(indirect_addr2)
                self.memory[indirect_addr2] = self.memory[indirect_addr1]
                
            elif cmd == "ADD":
                # ADD A B - Add value B to address A
                addr = int(parts[1])
                value = int(parts[2])
                self.check_memory_access(addr)
                self.memory[addr] += value
                
            elif cmd == "ADDI":
                # ADDI A1 A2 - Add contents of memory address A2 to address A1
                addr1 = int(parts[1])
                addr2 = int(parts[2])
                self.check_memory_access(addr1)
                self.check_memory_access(addr2)
                self.memory[addr1] += self.memory[addr2]
                
            elif cmd == "SUBI":
                # SUBI A1 A2 - Subtract contents of A2 from A1, put result in A2
                addr1 = int(parts[1])
                addr2 = int(parts[2])
                self.check_memory_access(addr1)
                self.check_memory_access(addr2)
                result = self.memory[addr1] - self.memory[addr2]
                self.memory[addr2] = result
                
            elif cmd == "JIF":
                # JIF A C - Jump to C if memory[A] <= 0
                addr = int(parts[1])
                jump_addr = int(parts[2])
                self.check_memory_access(addr)
                if self.memory[addr] <= 0:
                    self.memory[0] = jump_addr
                    return True
                    
            elif cmd == "PUSH":
                # PUSH A - Push memory A onto stack
                addr = int(parts[1])
                self.check_memory_access(addr)
                stack_ptr = self.memory[1]
                self.check_memory_access(stack_ptr)
                self.memory[stack_ptr] = self.memory[addr]
                self.memory[1] = stack_ptr - 1
                
            elif cmd == "POP":
                # POP A - Pop value from stack into memory A
                addr = int(parts[1])
                self.check_memory_access(addr)
                self.memory[1] += 1
                stack_ptr = self.memory[1]
                self.check_memory_access(stack_ptr)
                self.memory[addr] = self.memory[stack_ptr]
                
            elif cmd == "CALL":
                # CALL C - Call subroutine at C, push return address
                jump_addr = int(parts[1])
                # Return address'i stack'e push et
                stack_ptr = self.memory[1]
                self.check_memory_access(stack_ptr)
                self.memory[stack_ptr] = self.memory[0] + 1
                self.memory[1] = stack_ptr - 1
                self.memory[0] = jump_addr
                return True
                
            elif cmd == "RET":
                # RET - Return from subroutine by popping return address
                self.memory[1] += 1
                stack_ptr = self.memory[1]
                self.check_memory_access(stack_ptr)
                self.memory[0] = self.memory[stack_ptr]
                return True
                
            elif cmd == "HLT":
                # HLT - Halt CPU
                self.halted = True
                return False
                
            elif cmd == "USER":
                # USER A - Switch to user mode and jump to address contained at A
                addr = int(parts[1])
                self.check_memory_access(addr)
                self.kernel_mode = False
                jump_addr = self.memory[addr]
                self.memory[0] = jump_addr
                return True
                
            elif cmd == "SYSCALL":
                # SYSCALL PRN/HLT/YIELD
                if len(parts) < 2:
                    return True
                    
                # System call sırasında otomatik kernel mode
                prev_mode = self.kernel_mode
                self.kernel_mode = True
                
                if parts[1] == "PRN":
                    if len(parts) >= 3:
                        addr = int(parts[2])
                        self.check_memory_access(addr)
                        print(self.memory[addr])
                        # PRN system call 100 instruction boyunca bloklar
                        # Basit programlar için bloklamayı devre dışı bırak
                        if hasattr(main, 'thread_manager') and main.thread_manager.active:
                            self.blocked_until = self.memory[3] + 100
                        
                elif parts[1] == "HLT":
                    # Thread'i sonlandır - context switch gerekli
                    if hasattr(main, 'thread_manager') and main.thread_manager.active:
                        main.thread_manager.halt_current_thread(self)
                        # Eğer başka thread varsa devam et
                        return not self.halted
                    else:
                        self.halted = True
                        return False
                    
                elif parts[1] == "YIELD":
                    # Thread scheduling için
                    if hasattr(main, 'thread_manager') and main.thread_manager.active:
                        main.thread_manager.yield_cpu(self)
                        return True
                        
                # Önceki mode'a geri dön
                self.kernel_mode = prev_mode
                
            else:
                print(f"Unknown instruction: {cmd}", file=sys.stderr)
                
        except (ValueError, IndexError) as e:
            print(f"Error executing instruction '{instruction}': {e}", file=sys.stderr)
            self.halted = True
            return False
        except MemoryAccessError as e:
            print(f"Memory access error: {e}", file=sys.stderr)
            # User mode'da hatalı erişim - thread'i kapat
            if not self.kernel_mode and hasattr(main, 'thread_manager') and main.thread_manager.active:
                main.thread_manager.halt_current_thread(self)
            else:
                self.halted = True
            return False
            
        # PC'yi artır (jump instruction'ları için return True zaten yapıldı)
        self.memory[0] += 1
        return True
        
    def check_memory_access(self, addr):
        """Bellek erişim kontrolü"""
        if addr < 0 or addr >= len(self.memory):
            raise MemoryAccessError(f"Address {addr} out of bounds")
            
        # User mode'da sadece 1000+ erişilebilir (register'lar hariç)
        if not self.kernel_mode and addr >= 21 and addr < 1000:
            raise MemoryAccessError(f"User mode cannot access address {addr} (OS area)")
            
    def is_halted(self):
        return self.halted
        
    def print_memory(self, debug_mode):
        """Bellek içeriğini yazdır"""
        if debug_mode == 0 or debug_mode == 1:
            # Sadece 0 olmayan değerleri yazdır
            for i in range(len(self.memory)):
                if self.memory[i] != 0:
                    print(f"[{i}]: {self.memory[i]}", file=sys.stderr)
        elif debug_mode == 2:
            # Tüm bellek içeriğini yazdır (sınırlı)
            for i in range(min(1000, len(self.memory))):
                if self.memory[i] != 0:
                    print(f"[{i}]: {self.memory[i]}", file=sys.stderr)

class MemoryAccessError(Exception):
    pass

class ThreadManager:
    def __init__(self):
        self.threads = {}
        self.current_thread = 0
        self.active = False
        self.os_thread_id = 0
        
    def setup_threads(self, cpu):
        """Thread tablosunu kur"""
        # OS thread'i ekle
        self.threads[0] = {
            'id': 0,
            'state': 1,  # running
            'pc': cpu.memory[0],
            'stack_ptr': cpu.memory[1],
            'start_time': cpu.memory[3],
            'instructions_used': 0
        }
        
        # Thread table'ı memory'den oku (30'dan başlıyor)
        for i in range(1, 11):  # Thread 1-10
            base_addr = 30 + (i-1) * 10
            # Sadece geçerli thread ID'leri kabul et (1-10 arası)
            if base_addr < 130 and cpu.memory[base_addr] > 0 and cpu.memory[base_addr] <= 10:
                self.threads[cpu.memory[base_addr]] = {
                    'id': cpu.memory[base_addr],
                    'state': cpu.memory[base_addr + 1],  # 0=ready, 1=running, 2=blocked
                    'pc': cpu.memory[base_addr + 2],
                    'stack_ptr': cpu.memory[base_addr + 3],
                    'start_time': cpu.memory[3],
                    'instructions_used': 0
                }
                self.active = True
        
        # İlk thread'i başlat
        if self.active and len(self.threads) > 1:
            self.current_thread = 1
            self.switch_to_thread(cpu, 1)
            
    def yield_cpu(self, cpu):
        """SYSCALL YIELD işlemi"""
        if not self.active:
            return
        
        # Mevcut thread'in state'ini kaydet
        if self.current_thread in self.threads:
            thread = self.threads[self.current_thread]
            thread['pc'] = cpu.memory[0] + 1  # YIELD'dan sonraki instruction
            thread['stack_ptr'] = cpu.memory[1]
            thread['state'] = 0  # ready
        
        # Sonraki ready thread'i bul (round robin)
        next_thread = self.find_next_ready_thread()
    
        if next_thread is not None:
            self.switch_to_thread(cpu, next_thread)
        else:
            # Hiç ready thread yoksa, ilk thread'e dön
            if len(self.threads) > 1:
                first_thread = min(t for t in self.threads.keys() if t > 0)
                self.threads[first_thread]['state'] = 0  # ready yap
                self.switch_to_thread(cpu, first_thread)
            
    def find_next_ready_thread(self):
        """Sonraki çalışmaya hazır thread'i bul"""
        # Round robin - current'tan sonrakine bak
        thread_ids = sorted(self.threads.keys())
    
        # Eğer current thread hala varsa onun index'ini bul
        if self.current_thread in thread_ids:
            current_idx = thread_ids.index(self.current_thread)
        
            # Önce current'tan sonrakilere bak
            for i in range(current_idx + 1, len(thread_ids)):
                tid = thread_ids[i]
                if tid != 0 and self.threads[tid]['state'] == 0:  # ready
                    return tid
                
            # Baştan current'a kadar bak
            for i in range(0, current_idx):
                tid = thread_ids[i]
                if tid != 0 and self.threads[tid]['state'] == 0:  # ready
                    return tid
        else:
            # Current thread silinmiş, ilk ready thread'i bul
            for tid in thread_ids:
                if tid != 0 and self.threads[tid]['state'] == 0:  # ready
                    return tid
                
        # Hiç ready thread yoksa None döndür
        return None
        
    def switch_to_thread(self, cpu, thread_id):
        """Thread'e geçiş yap"""
        if thread_id not in self.threads:
            return
        
        self.current_thread = thread_id
        thread = self.threads[thread_id]
        thread['state'] = 1  # running
    
        # Context restore
        print(f"Switching to Thread {thread_id}, PC={thread['pc']}", file=sys.stderr)  # DEBUG
        cpu.memory[0] = thread['pc']
        cpu.memory[1] = thread['stack_ptr']
    
        # Thread 0 (OS) ise kernel mode, diğerleri user mode
        cpu.kernel_mode = (thread_id == 0)
    
        print(f"Context switch to Thread {thread_id}", file=sys.stderr)
        
    def halt_current_thread(self, cpu):
        """Mevcut thread'i sonlandır"""
        halted_thread = self.current_thread
    
        # Önce bir sonraki thread'i bul
        next_thread = self.find_next_ready_thread()
    
        # Şimdi thread'i sil
        if halted_thread in self.threads:
            del self.threads[halted_thread]
            print(f"Thread {halted_thread} halted", file=sys.stderr)
    
        # Başka thread varsa ona geç
        if next_thread is not None:
            self.switch_to_thread(cpu, next_thread)
            # CPU'yu halt etme, devam et!
        elif len(self.threads) > 1:  # OS thread hala var mı?
            # Sadece OS kaldı
            self.switch_to_thread(cpu, 0)
        else:
            # Hiç thread kalmadı
            cpu.halted = True
            
    def update_thread_stats(self, cpu):
        """Thread istatistiklerini güncelle"""
        if self.current_thread in self.threads:
            self.threads[self.current_thread]['instructions_used'] += 1
            
    def print_thread_table(self):
        """Thread tablosunu yazdır"""
        print("\n--- Thread Table ---", file=sys.stderr)
        for tid, thread in sorted(self.threads.items()):
            state_names = {0: 'ready', 1: 'running', 2: 'blocked'}
            state_name = state_names.get(thread['state'], 'unknown')
            print(f"Thread {tid}: state={state_name}, pc={thread['pc']}, "
                  f"stack_ptr={thread['stack_ptr']}, instructions={thread['instructions_used']}", 
                  file=sys.stderr)

def main():
    parser = argparse.ArgumentParser(description='GTU-C312 CPU Simulator')
    parser.add_argument('filename', help='Program file to execute')
    parser.add_argument('-D', '--debug', type=int, default=0, 
                       help='Debug mode (0-3)')
    
    args = parser.parse_args()
    
    cpu = GTUC312CPU()
    thread_manager = ThreadManager()
    
    try:
        cpu.load_program(args.filename)
    except FileNotFoundError:
        print(f"Error: File '{args.filename}' not found", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Error loading program: {e}", file=sys.stderr)
        return 1
    
    # Check if this is an OS program and setup threads
    if any("USER" in instr for instr in cpu.instructions.values()):
        thread_manager.setup_threads(cpu)
        thread_manager.active = True
        print("Operating System mode enabled", file=sys.stderr)
    
    print(f"Starting GTU-C312 CPU simulation...")
    print(f"Debug mode: {args.debug}")
    
    instruction_count = 0
    
    # Global reference for SYSCALL handling
    main.thread_manager = thread_manager
    
    while not cpu.is_halted():
        try:
            if args.debug == 2:
                # Debug mode 2: Her instruction sonrası bekle
                print(f"\nNext instruction at PC={cpu.memory[0]}: {cpu.instructions.get(cpu.memory[0], 'N/A')}")
                input("Press Enter to continue...")
                
            if not cpu.execute():
                break
                
            instruction_count += 1
            
            # Thread istatistiklerini güncelle
            if thread_manager.active:
                thread_manager.update_thread_stats(cpu)
            
            if args.debug == 1:
                # Debug mode 1: Her instruction sonrası memory dump
                print(f"\n--- After instruction {instruction_count} ---", file=sys.stderr)
                cpu.print_memory(1)
                
            elif args.debug == 3:
                # Debug mode 3: Context switch/syscall'larda thread table
                # YIELD veya thread değişiminde yazdır
                if thread_manager.active and instruction_count % 50 == 0:
                    thread_manager.print_thread_table()
                
        except KeyboardInterrupt:
            print("\nSimulation interrupted by user", file=sys.stderr)
            break
        except Exception as e:
            print(f"Runtime error: {e}", file=sys.stderr)
            break
    
    print(f"\nSimulation completed. Total instructions executed: {instruction_count}")
    
    if args.debug == 0:
        # Debug mode 0: Program bitince memory dump
        print("\n--- Final Memory State ---", file=sys.stderr)
        cpu.print_memory(0)
    
    if thread_manager.active and args.debug >= 0:
        thread_manager.print_thread_table()
    
    return 0

if __name__ == "__main__":
    sys.exit(main())