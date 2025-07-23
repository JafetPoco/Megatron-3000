#ifndef CLI_H
#define CLI_H

#include <string>

/**
 * @file cli.h
 * @brief Declaraciones de funciones para el intérprete de comandos SQL y de disco (CLI).
 * 
 * Este archivo contiene los prototipos de funciones necesarias para manejar comandos
 * ingresados por el usuario en la línea de comandos. Incluye soporte para comandos
 * SQL simulados y comandos relacionados con el disco.
 * 
 * @author Jafet Poco
 * @author Berly Dueñas
 */

/// @brief Función principal del CLI que inicia el intérprete interactivo.
/// @return Código de salida (0 normalmente).
/// @author Jafet Poco
int main_cli();

/**
 * @brief Maneja la ejecución de una instrucción SELECT.
 * @param sql Instrucción SQL completa.
 * @author Jafet Poco
 */
void handle_select(const std::string &sql);

/**
 * @brief Maneja la ejecución de una instrucción DELETE.
 * @param sql Instrucción SQL completa.
 * @author Berly Dueñas
 */
void handle_delete(const std::string &sql);

/**
 * @brief Maneja la ejecución de una instrucción INSERT.
 * @param sql Instrucción SQL completa.
 * @author Jafet Poco
 */
void handle_insert(const std::string &sql);

/**
 * @brief Maneja la ejecución del comando addcsv.
 * @param sql Instrucción completa que incluye nombre del archivo y nombre de tabla.
 * @author Berly Dueñas
 */
void handle_addcsv(const std::string &sql);

/**
 * @brief Muestra la ayuda con los comandos soportados.
 * @param dummy Se ignora, solo para mantener la firma consistente.
 * @author Jafet Poco
 */
void handle_help(const std::string &dummy);

/**
 * @brief Maneja comandos relacionados con el disco (.disk open, .disk write, .disk info).
 * @param str Comando completo ingresado por el usuario.
 * @author Berly Dueñas
 */
void handle_disk_command(const std::string &str);

void handle_buffer_command(const std::string &str);

void handle_schema_command(const std::string &str);

void handle_file_command(const std::string &str);


#endif // CLI_H

