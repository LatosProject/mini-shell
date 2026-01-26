#!/usr/bin/env python3
"""
Mini-shell (.msh) 文件格式化工具
支持的语法:
- 变量赋值: x = 10;
- 数组: arr[5] = {1, 2, 3};
- 控制流: if/while condition { body }
- 命令: echo, source, run
- 注释: # comment
"""

import re
import sys

class MshFormatter:
    def __init__(self):
        self.indent_size = 4
        self.operators = ['==', '!=', '>=', '<=', '>', '<', '+', '-', '*', '/', '%']
        self.keywords = ['if', 'while', 'echo', 'source', 'run']

    def format_file(self, filepath):
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        formatted = self.format_content(content)

        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(formatted)

        return formatted

    def format_content(self, content):
        lines = content.split('\n')
        result = []

        for line in lines:
            formatted_line = self.format_line(line)
            result.append(formatted_line)

        # 移除末尾多余空行，保留一个
        while len(result) > 1 and result[-1] == '' and result[-2] == '':
            result.pop()

        return '\n'.join(result)

    def format_line(self, line):
        # 保留空行
        stripped = line.strip()
        if not stripped:
            return ''

        # 保留注释行（只处理前导空格）
        if stripped.startswith('#'):
            return stripped

        # 格式化代码行
        return self.format_code_line(stripped)

    def format_code_line(self, line):
        # 处理包含 { } 的控制流语句
        if '{' in line and '}' in line:
            return self.format_control_flow(line)

        # 处理普通语句
        return self.format_statement(line)

    def format_control_flow(self, line):
        """格式化 while/if 语句，展开花括号内容"""
        # 匹配 while/if condition { body }
        match = re.match(r'(while|if)\s+(.+?)\s*\{(.+)\}', line)
        if not match:
            return self.format_statement(line)

        keyword = match.group(1)
        condition = self.format_condition(match.group(2).strip())
        body = match.group(3).strip()

        # 格式化 body 中的多条语句
        statements = self.split_statements(body)

        if len(statements) == 1:
            # 单条语句，保持单行
            formatted_body = self.format_statement(statements[0])
            return f'{keyword} {condition} {{\n    {formatted_body}\n}}'
        else:
            # 多条语句，每条一行
            formatted_statements = []
            for stmt in statements:
                formatted = self.format_statement(stmt)
                if formatted:
                    formatted_statements.append(f'    {formatted}')

            body_str = '\n'.join(formatted_statements)
            return f'{keyword} {condition} {{\n{body_str}\n}}'

    def split_statements(self, body):
        """分割多条语句"""
        statements = []
        current = ''
        depth = 0

        for char in body:
            if char == '{':
                depth += 1
                current += char
            elif char == '}':
                depth -= 1
                current += char
            elif char == ';' and depth == 0:
                if current.strip():
                    statements.append(current.strip())
                current = ''
            else:
                current += char

        if current.strip():
            statements.append(current.strip())

        return statements

    def format_condition(self, condition):
        """格式化条件表达式"""
        result = condition

        # 在比较运算符周围添加空格
        for op in ['==', '!=', '>=', '<=']:
            result = re.sub(rf'\s*{re.escape(op)}\s*', f' {op} ', result)

        # 单字符比较运算符（避免影响 ==, !=, >=, <=）
        result = re.sub(r'(?<![=!<>])\s*>\s*(?!=)', ' > ', result)
        result = re.sub(r'(?<![=!<>])\s*<\s*(?!=)', ' < ', result)

        return result.strip()

    def format_statement(self, stmt):
        """格式化单条语句"""
        stmt = stmt.strip()
        if not stmt:
            return ''

        # 确保语句以分号结尾
        if not stmt.endswith(';') and not stmt.endswith('}'):
            stmt += ';'

        # 格式化赋值语句: x = 10;
        match = re.match(r'(\w+)\s*=\s*(.+);$', stmt)
        if match:
            var = match.group(1)
            value = self.format_expression(match.group(2).strip())
            return f'{var} = {value};'

        # 格式化数组赋值: arr[i] = value;
        match = re.match(r'(\w+)\s*\[\s*(\w+)\s*\]\s*=\s*(.+);$', stmt)
        if match:
            arr = match.group(1)
            index = match.group(2)
            value = self.format_expression(match.group(3).strip())
            return f'{arr}[{index}] = {value};'

        # 格式化数组声明: arr[5] = {1, 2, 3};
        match = re.match(r'(\w+)\s*\[\s*(\w+)\s*\]\s*=\s*\{(.+)\};$', stmt)
        if match:
            arr = match.group(1)
            size = match.group(2)
            values = match.group(3)
            formatted_values = ', '.join(v.strip() for v in values.split(','))
            return f'{arr}[{size}] = {{{formatted_values}}};'

        # 格式化 echo 语句
        match = re.match(r'echo\s+(.+);$', stmt)
        if match:
            arg = match.group(1).strip()
            # 处理数组访问 echo arr[i];
            arr_match = re.match(r'(\w+)\s*\[\s*(\w+)\s*\]', arg)
            if arr_match:
                return f'echo {arr_match.group(1)}[{arr_match.group(2)}];'
            return f'echo {arg};'

        # 格式化 source/run 语句
        match = re.match(r'(source|run)\s+(.+);$', stmt)
        if match:
            cmd = match.group(1)
            arg = match.group(2).strip()
            return f'{cmd} {arg};'

        return stmt

    def format_expression(self, expr):
        """格式化表达式"""
        result = expr.strip()

        # 在运算符周围添加空格（使用更精确的正则）
        # 处理 +, -, *, /, % 运算符
        result = re.sub(r'(\w+)\s*\+\s*(\w+)', r'\1 + \2', result)
        result = re.sub(r'(\w+)\s*-\s*(\w+)', r'\1 - \2', result)
        result = re.sub(r'(\w+)\s*\*\s*(\w+)', r'\1 * \2', result)
        result = re.sub(r'(\w+)\s*/\s*(\w+)', r'\1 / \2', result)
        result = re.sub(r'(\w+)\s*%\s*(\w+)', r'\1 % \2', result)

        return result.strip()


def main():
    if len(sys.argv) < 2:
        print("Usage: msh-format.py <file.msh>")
        sys.exit(1)

    filepath = sys.argv[1]
    formatter = MshFormatter()

    try:
        formatted = formatter.format_file(filepath)
        print(f"Formatted: {filepath}")
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
