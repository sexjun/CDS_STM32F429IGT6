import subprocess
import os
import sys
import platform

class PyOCDWrapper:
    def __init__(self):
        self.target_chip = "stm32f429xg"
        self.elf_file = r"D:\github\CDS_STM32F429IGT6\cmake-build-debug\yehuo_STM32F429IGT6_base_preject.elf"

    def run_command(self, command):
        """执行系统命令并返回输出"""
        try:
            print(f"执行命令: {' '.join(command)}")
            # 根据不同操作系统设置shell参数
            shell_flag = True if platform.system() == "Windows" else False
            result = subprocess.run(
                command,
                shell=shell_flag,
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            print("命令执行成功")
            if result.stdout:
                print("输出:")
                print(result.stdout)
            return True
        except subprocess.CalledProcessError as e:
            print(f"命令执行失败: {e.stderr}")
            return False
        except Exception as e:
            print(f"发生错误: {e}")
            return False

    def list_devices(self):
        """列出连接的设备"""
        return self.run_command(["pyocd", "list"])

    def erase_chip(self):
        """擦除芯片"""
        return self.run_command(["pyocd", "erase", "-t", self.target_chip, "--chip"])

    def flash_program(self):
        """下载程序"""
        if not os.path.exists(self.elf_file):
            print(f"错误: 文件不存在: {self.elf_file}")
            return False
        return self.run_command(["pyocd", "flash", "-t", self.target_chip, self.elf_file])

    def reset_chip(self):
        """复位芯片"""
        return self.run_command(["pyocd", "reset", "-t", self.target_chip])

    def flash_and_reset(self):
        """组合操作：擦除、下载、复位"""
        if not os.path.exists(self.elf_file):
            print(f"错误: 文件不存在: {self.elf_file}")
            return False
        return self.run_command([
            "pyocd", "flash", "-t", self.target_chip,
            "--erase", "chip", self.elf_file, "-R"
        ])

    def set_target_chip(self, chip):
        """设置目标芯片"""
        self.target_chip = chip
        print(f"目标芯片已设置为: {chip}")

    def set_elf_file(self, file_path):
        """设置ELF文件路径"""
        self.elf_file = file_path
        print(f"ELF文件路径已设置为: {file_path}")

def display_menu():
    """显示主菜单"""
    print("\n===== PyOCD命令菜单 =====")
    print("1. 列出连接的设备")
    print("2. 设置目标芯片")
    print("3. 设置ELF文件路径")
    print("4. 擦除芯片")
    print("5. 下载程序")
    print("6. 复位芯片")
    print("7. 擦除、下载并复位")
    print("8. 退出")
    print("=======================\n")

def main():
    """主函数"""
    wrapper = PyOCDWrapper()

    while True:
        display_menu()
        choice = input("请输入你的选择 (1-8): ")

        if choice == "1":
            wrapper.list_devices()

        elif choice == "2":
            chip = input(f"请输入目标芯片 (当前: {wrapper.target_chip}): ")
            if chip:
                wrapper.set_target_chip(chip)

        elif choice == "3":
            file_path = input(f"请输入ELF文件路径 (当前: {wrapper.elf_file}): ")
            if file_path:
                wrapper.set_elf_file(file_path)

        elif choice == "4":
            wrapper.erase_chip()

        elif choice == "5":
            wrapper.flash_program()

        elif choice == "6":
            wrapper.reset_chip()

        elif choice == "7":
            wrapper.flash_and_reset()

        elif choice == "8":
            print("退出程序")
            break

        else:
            print("无效的选择，请输入1-8之间的数字")

if __name__ == "__main__":
    main()