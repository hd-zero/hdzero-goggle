#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import subprocess
import os
import glob

# --- Configuración ---
target_font_file = 'Montserrat-Medium.ttf'  # Fuente TTF
output_dir = 'out'  # Directorio de salida

# Caracteres españoles a agregar (puntos Unicode)
spanish_unicode_points = [161, 191, 193, 201, 205, 209, 211, 218, 220, 225, 233, 237, 241, 243, 250, 252]

def extract_simplified_chinese_unicode():
    """Extrae caracteres chinos del archivo zh_hans.ini"""
    input_file_path = "../../mkapp/app/language/zh_hans.ini"
    range_str = ""
    char_pattern = re.compile(r'[\u4e00-\u9fff]')
    unique_chars = set()

    try:
        with open(input_file_path, "r", encoding="utf-8") as file:
            for line in file:
                for char in line:
                    if char_pattern.match(char):
                        unique_chars.add(char)

        for char in sorted(unique_chars):
            range_str += f"{ord(char)},"

        return range_str[:-1]
    except FileNotFoundError:
        print(f"Warning: {input_file_path} not found. Using empty Chinese range.")
        return ""

def update_range_with_spanish(original_range):
    """Actualiza un rango agregando los caracteres españoles"""
    if not original_range:
        return original_range
    
    # Convertir el rango original a lista de números
    existing_points = set()
    for part in original_range.split(','):
        if '-' in part:
            start, end = map(int, part.split('-'))
            existing_points.update(range(start, end + 1))
        else:
            existing_points.add(int(part))
    
    # Agregar los caracteres españoles
    new_points = existing_points.union(set(spanish_unicode_points))
    
    # Convertir de vuelta a formato de rango
    sorted_points = sorted(new_points)
    ranges = []
    i = 0
    while i < len(sorted_points):
        start = sorted_points[i]
        while i + 1 < len(sorted_points) and sorted_points[i + 1] == sorted_points[i] + 1:
            i += 1
        end = sorted_points[i]
        
        if start == end:
            ranges.append(str(start))
        else:
            ranges.append(f"{start}-{end}")
        i += 1
    
    return ','.join(ranges)

def process_font_file(input_file):
    """Procesa un archivo de fuente individual"""
    output_file = os.path.join(output_dir, os.path.basename(input_file))
    
    print(f"\nProcesando: {input_file}")
    
    try:
        # Extraer la línea de comando original del archivo C
        with open(input_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Buscar la línea Opts que contiene el comando original
        opts_match = re.search(r'\*\s*Opts:\s*(.*)', content)
        if not opts_match:
            print(f"  Warning: No se encontró la línea 'Opts:' en {input_file}")
            return False
        
        original_command = opts_match.group(1).strip()
        
        # Extraer el rango actual de Montserrat
        montserrat_range_match = re.search(r'--font\s+Montserrat-Medium\.ttf\s+--range\s+([\d,-]+)', original_command)
        if not montserrat_range_match:
            print(f"  Warning: No se encontró el rango de Montserrat en {input_file}")
            return False
        
        original_range = montserrat_range_match.group(1)
        print(f"  Rango original: {original_range}")
        
        # Actualizar el rango con caracteres españoles
        updated_range = update_range_with_spanish(original_range)
        print(f"  Rango actualizado: {updated_range}")
        
        # Actualizar el comando original con cuidado
        new_command = original_command.replace(
            f"--font Montserrat-Medium.ttf --range {original_range}",
            f"--font Montserrat-Medium.ttf --range {updated_range}"
        )
        
        # Actualizar la ruta de salida
        basename = os.path.basename(input_file)
        new_command = new_command.replace(
            f"-o out/{basename}",
            f"-o {output_file}"
        )
        
        print(f"  Generando nueva fuente...")
        # Usar subprocess.run sin shell=True para evitar problemas de parsing
        cmd_parts = ['lv_font_conv'] + new_command.split()
        result = subprocess.run(cmd_parts, capture_output=True, text=True)
        
        if result.returncode == 0:
            print(f"  ✓ Fuente generada exitosamente: {output_file}")
            return True
        else:
            print(f"  ✗ Error al generar la fuente: {result.stderr}")
            if result.stdout:
                print(f"    Output: {result.stdout}")
            return False
            
    except Exception as e:
        print(f"  ✗ Error: {e}")
        return False

def main():
    # Crear directorio de salida si no existe
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Creado directorio de salida: {output_dir}")
    
    # Encontrar todos los archivos lv_font_montserrat_*.c
    font_files = glob.glob("lv_font_montserrat_*.c")
    
    if not font_files:
        print("No se encontraron archivos lv_font_montserrat_*.c en el directorio actual")
        return
    
    print(f"Encontrados {len(font_files)} archivos de fuente:")
    for f in font_files:
        print(f"  - {f}")
    
    # Procesar cada archivo
    success_count = 0
    failed_count = 0
    
    for font_file in font_files:
        if process_font_file(font_file):
            success_count += 1
        else:
            failed_count += 1
    
    # Resumen
    print("\n" + "="*50)
    print(f"Procesamiento completado:")
    print(f"  ✓ Exitosos: {success_count}")
    print(f"  ✗ Fallidos: {failed_count}")
    print(f"  Total: {len(font_files)}")
    print("="*50)
    
    if success_count > 0:
        print(f"\nLas fuentes actualizadas están en: {output_dir}/")
        print("Recuerda copiar los archivos al directorio de fuentes del proyecto:")
        print(f"  cp {output_dir}/* ../../lib/lvgl/lvgl/src/font/")

if __name__ == "__main__":
    main()