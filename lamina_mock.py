#!/usr/bin/env python3
"""
简单的 Lamina 解释器模拟器
用于测试 LaminaLab IDE 的代码执行功能
"""

import sys
import time
import os

def mock_lamina_interpreter(file_path):
    """模拟 Lamina 解释器执行"""
    print(f"Lamina Mock Interpreter v1.0")
    print(f"Executing file: {file_path}")
    print("-" * 40)
    
    if not os.path.exists(file_path):
        print(f"Error: File '{file_path}' not found!", file=sys.stderr)
        return 1
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        print("File content loaded successfully")
        print("Simulating Lamina code execution...")
        time.sleep(0.5)  # 模拟执行时间
        
        # 简单解析和模拟执行
        lines = content.split('\n')
        for line_num, line in enumerate(lines, 1):
            line = line.strip()
            if not line or line.startswith('//'):
                continue
                
            if line.startswith('print('):
                # 模拟 print 语句
                message = line[6:-1].strip('"')
                print(f"[Lamina Output] {message}")
            elif line.startswith('var '):
                # 模拟变量声明
                var_part = line[4:].split('=')[0].strip()
                print(f"[Lamina] Variable declared: {var_part}")
            elif line.startswith('for '):
                # 模拟循环
                print(f"[Lamina] Executing loop...")
            
            time.sleep(0.1)  # 模拟执行延迟
        
        print("-" * 40)
        print("Lamina execution completed successfully!")
        return 0
        
    except Exception as e:
        print(f"Error: {str(e)}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: lamina_mock.py <script.lm>", file=sys.stderr)
        sys.exit(1)
    
    file_path = sys.argv[1]
    exit_code = mock_lamina_interpreter(file_path)
    sys.exit(exit_code)
