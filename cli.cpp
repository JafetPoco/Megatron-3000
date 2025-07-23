#include <algorithm>
#include "cli.h"
#include "block.h"
#include "disk.h"
#include "globals.h"
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using SQLHandler = void (*)(const std::string &);
static const std::unordered_map<std::string, SQLHandler> sql_handlers = {
    {"select", handle_select},
    {"delete", handle_delete},
    {"insert", handle_insert},
    {"addcsv", handle_addcsv},
    { "help", handle_help },
};

std::string to_lower(const std::string &s) {
  std::string out = s;
  std::transform(out.begin(), out.end(), out.begin(), ::tolower);
  return out;
}

std::string trim(const std::string &s) {
  auto start = s.find_first_not_of(" \t");
  auto end = s.find_last_not_of(" \t;");
  return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim))
    elems.push_back(trim(item));
  return elems;
}

int main_cli() {
  using_history();
  //maximo de entradas en el historial
  stifle_history(15);

  std::cout << "Megatron 3000 "<< std::endl;
  std::cout << "escribir \"help\" para ver los comandos disponibles" << std::endl;

  while (true) {
    char *raw = readline("> ");
    if (!raw)
      break; // Ctrl+D
    std::string line = trim(raw);
    free(raw);
    if (line.empty())
      continue;
    //COMANDOS
    if (to_lower(line) == "exit")
      break;
    add_history(line.c_str());

    //addcsv
    if (line.rfind("addcsv", 0) == 0) {
      handle_addcsv(line);
      continue;
    }
    //disk ops
    if (line.rfind(".disk", 0) == 0) {
      handle_disk_command(line);
      continue;
    }

    //buffer
    if (line.rfind(".buffer", 0) == 0) {
      handle_buffer_command(line);
      continue;
    }

    //schema
    if (line.rfind(".schema", 0) == 0) {
      handle_schema_command(line);
      continue;
    }

    // Extraer comando inicial
    std::stringstream ss(line);
    std::string cmd;
    ss >> cmd;
    std::string cmd_lc = to_lower(cmd);
    auto it = sql_handlers.find(cmd_lc);
    if (it != sql_handlers.end()) {
      it->second(line);
    } else {
      std::cerr << "Comando desconocido: " << cmd << std::endl;
    }
  }
  std::cout << "Saliendo..." << std::endl;
  return 0;
}

// Handler SELECT: "SELECT cols FROM table [WHERE cond]"
void handle_select(const std::string &sql) {
  std::string low = to_lower(sql);
  size_t pos_from = low.find(" from ");
  if (pos_from == std::string::npos) {
    std::cerr << "Error: falta FROM en SELECT." << std::endl;
    return;
  }
  std::string cols = trim(sql.substr(6, pos_from - 6));
  size_t pos_where = low.find(" where ", pos_from);
  std::string table =
      (pos_where == std::string::npos)
          ? trim(sql.substr(pos_from + 6))
          : trim(sql.substr(pos_from + 6, pos_where - (pos_from + 6)));
  std::string cond;
  if (pos_where != std::string::npos) {
    cond = trim(sql.substr(pos_where + 7));
  }
  std::cout << "SELECT columnas: " << cols << "\nFROM tabla: " << table;
  if (!cond.empty())
    std::cout << " | WHERE: " << cond;
  std::cout << std::endl;
}

// Handler DELETE: "DELETE FROM table [WHERE cond]"
void handle_delete(const std::string &sql) {
  std::string low = to_lower(sql);
  size_t pos_from = low.find(" from ");
  if (pos_from == std::string::npos) {
    std::cerr << "Error: falta FROM en DELETE." << std::endl;
    return;
  }
  size_t pos_where = low.find(" where ", pos_from);
  std::string table =
      (pos_where == std::string::npos)
          ? trim(sql.substr(pos_from + 6))
          : trim(sql.substr(pos_from + 6, pos_where - (pos_from + 6)));
  std::string cond;
  if (pos_where != std::string::npos) {
    cond = trim(sql.substr(pos_where + 7));
  }
  std::cout << "DELETE FROM tabla: " << table;
  if (!cond.empty())
    std::cout << " | WHERE: " << cond;
  std::cout << std::endl;
}

// Handler INSERT: "INSERT INTO table (cols) VALUES (vals)"
void handle_insert(const std::string &sql) {
  std::string low = to_lower(sql);
  size_t pos_into = low.find(" into ");
  size_t pos_vals = low.find(" values ");
  if (pos_into == std::string::npos || pos_vals == std::string::npos) {
    std::cerr << "Error: sintaxis INSERT inválida." << std::endl;
    return;
  }
  std::string table_and_cols =
      trim(sql.substr(pos_into + 6, pos_vals - (pos_into + 6)));
  size_t paren = table_and_cols.find('(');
  std::string table = (paren == std::string::npos)
                          ? table_and_cols
                          : trim(table_and_cols.substr(0, paren));
  std::string cols_part =
      (paren == std::string::npos) ? "" : trim(table_and_cols.substr(paren));
  std::string vals_part = trim(sql.substr(pos_vals + 8));
  std::cout << "INSERT INTO tabla: " << table;
  if (!cols_part.empty())
    std::cout << " | Columnas: " << cols_part;
  std::cout << " | Valores: " << vals_part << std::endl;
}

// Handler ADDCSV: "addcsv archivo.csv tabla"
void handle_addcsv(const std::string &sql) {
  auto parts = split(sql, ' ');
  if (parts.size() < 3) {
    std::cerr << "Error: addcsv requiere archivo y tabla." << std::endl;
    return;
  }
  std::string archivo = parts[1];
  std::string tabla = parts[2];
  std::cout << "Importar CSV: archivo=" << archivo << " | tabla=" << tabla
            << std::endl;
}

void handle_help(const std::string&) {
  std::cout << "Comandos soportados:\n\n";

  std::cout << "SQL:\n";
  std::cout << "  SELECT columnas FROM tabla [WHERE condición];\n";
  std::cout << "  DELETE FROM tabla [WHERE condición];\n";
  std::cout << "  INSERT INTO tabla (col1, col2, ...) VALUES (val1, val2, ...);\n";
  std::cout << "  addcsv archivo.csv tabla       # Importar CSV en una tabla\n\n";

  std::cout << "Comandos especiales:\n";

  std::cout << "  .disk open block <n>           # Abrir bloque desde disco\n";
  std::cout << "  .disk open sector <n>          # Abrir sector desde disco\n";
  std::cout << "  .disk write <bloque> <datos>   # Escribir datos en un bloque\n";
  std::cout << "  .disk info                     # Mostrar info del disco\n\n";

  std::cout << "  .buffer show                   # Mostrar contenido del buffer pool\n";
  std::cout << "  .buffer read <n>               # Leer página/bloque en buffer\n";
  std::cout << "  .buffer type                   # Mostrar estrategia de reemplazo\n";
  std::cout << "  .buffer stats                  # Estadísticas del buffer\n\n";

  std::cout << "  .schema addcsv archivo tabla   # Registrar esquema a partir de CSV\n";
  std::cout << "  .schema print                  # Mostrar tablas registradas\n\n";

  std::cout << "  .file open <archivo>           # Abrir archivo\n";
  std::cout << "  .file find <archivo>           # Buscar archivo\n";
  std::cout << "  .file size <archivo>           # Mostrar tamaño de archivo\n\n";

  std::cout << "General:\n";
  std::cout << "  help                           # Mostrar esta ayuda\n";
  std::cout << "  exit                           # Salir del programa\n\n";

  std::cout << "Notas:\n";
  std::cout << "  - Usa las flechas ↑ ↓ para navegar por el historial.\n";
  std::cout << "  - Todos los comandos son insensibles a mayúsculas.\n";
}

void handle_disk_command(const std::string &str) {
  auto parts = split(str, ' ');
  if (parts.size() < 2) {
    std::cerr << "Error: comando .disk incompleto\n";
    return;
  }

  const std::string &subcmd = parts[1];

  //OPEN 
  if (subcmd == "open") {
    if (parts.size() < 3) {
      std::cerr << "Uso: .disk open [block|sector] <número>\n";
      return;
    }

    const std::string &entity = to_lower(parts[2]);

    if (entity == "block") {
      if (parts.size() < 4) {
        std::cerr << "Error: .disk open block requiere un número de bloque\n";
        return;
      }

      try {
        int block_id = std::stoi(parts[3]);
        std::cout << "[DISK] Abrir bloque " << block_id << "\n";

        Block page(block_id);

        std::cout << page.getData()<<'\n';

      } catch (const std::invalid_argument &e) {
        std::cerr << "Error: \"" << parts[3] << "\" no es un número válido.\n";
        return;
      } catch (const std::out_of_range &e) {
        std::cerr << "Error: número de bloque fuera de rango.\n";
        return;
      }

    } 
    else if (entity == "sector") {
      if (parts.size() < 4) {
        std::cerr << "Error: .disk open sector requiere un número de sector\n";
        return;
      }

      try {
        int sector_id = std::stoi(parts[3]);
        std::cout << "[DISK] Abrir sector " << sector_id << "\n";

        std::cout <<disk->readSector(sector_id) <<"\n";

      } catch (const std::invalid_argument &e) {
        std::cerr << "Error: \"" << parts[3] << "\" no es un número válido.\n";
        return;
      } catch (const std::out_of_range &e) {
        std::cerr << "Error: número de sector fuera de rango.\n";
        return;
      }

    } else {
      std::cerr << "Error: entidad desconocida \"" << parts[2] << "\". Usa 'block' o 'sector'.\n";
      return;
    }
  }
  //WRITE
  else if (subcmd == "write") {
    if (parts.size() < 4) {
      std::cerr << "Error: .disk write requiere bloque y datos\n";
      return;
    }
    int block = std::stoi(parts[2]);

    // Extraer todo después del número de bloque
    size_t data_pos = str.find(parts[2]) + parts[2].size();
    std::string data = trim(str.substr(data_pos));

    // TODO: Llamar a la función real para escribir el bloque
    // Ejemplo: disk.write_block(block, data);
    std::cout << "[DISK] Escribir en bloque " << block << ": \"" << data << "\"\n";

  } else if (subcmd == "info") {
    // TODO: Mostrar información real del disco
    // Ejemplo: disk.print_info();
    std::cout << "[DISK] Información del disco:\n";
    std::cout << "[SIMULACIÓN] Total de bloques usados: (n)\n";

  } else {
    std::cerr << "Error: subcomando .disk desconocido\n";
  }
}

void handle_schema_command(const std::string &str) {
  auto parts = split(str, ' ');
  if (parts.size() < 2) {
    std::cerr << "Uso: .schema [addcsv <archivo> <tabla> | print]\n";
    return;
  }

  const std::string &subcmd = to_lower(parts[1]);

  if (subcmd == "addcsv") {
    if (parts.size() < 4) {
      std::cerr << "Uso: .schema addcsv <archivo.csv> <nombre_tabla>\n";
      return;
    }
    const std::string &archivo = parts[2];
    const std::string &tabla = parts[3];

    // TODO: Llamar schema_addcsv(archivo, tabla);
    std::cout << "[SCHEMA] Agregar CSV: archivo=\"" << archivo
              << "\", tabla=\"" << tabla << "\"\n";

  } else if (subcmd == "print") {
    // TODO: Llamar schema_print();
    std::cout << "[SCHEMA] Mostrar todas las tablas registradas\n";

  } else {
    std::cerr << "Subcomando .schema desconocido\n";
  }
}

void handle_buffer_command(const std::string &str) {
  auto parts = split(str, ' ');
  if (parts.size() < 2) {
    std::cerr << "Uso: .buffer [show|read <id>|type|stats]\n";
    return;
  }

  const std::string &subcmd = to_lower(parts[1]);

  if (subcmd == "show") {
    // TODO: Llamar buffer_show();
    std::cout << "[BUFFER] Mostrar contenido del buffer\n";

  } else if (subcmd == "read") {
    if (parts.size() < 3) {
      std::cerr << "Uso: .buffer read <bloque_id>\n";
      return;
    }
    try {
      int id = std::stoi(parts[2]);
      // TODO: Llamar buffer_read(id);
      std::cout << "[BUFFER] Leer bloque " << id << "\n";
    } catch (...) {
      std::cerr << "Error: bloque_id inválido.\n";
    }

  } else if (subcmd == "type") {
    // TODO: Llamar buffer_show_type();
    std::cout << "[BUFFER] Mostrar política de reemplazo\n";

  } else if (subcmd == "stats") {
    // TODO: Llamar buffer_show_stats();
    std::cout << "[BUFFER] Mostrar estadísticas del buffer\n";

  } else {
    std::cerr << "Subcomando .buffer desconocido\n";
  }
}

void handle_file_command(const std::string &str) {
  auto parts = split(str, ' ');
  if (parts.size() < 3) {
    std::cerr << "Uso: .file [open|find|size] <nombre_archivo>\n";
    return;
  }

  const std::string &subcmd = to_lower(parts[1]);
  const std::string &filename = parts[2];

  if (subcmd == "open") {
    // TODO: Llamar file_open(filename);
    std::cout << "[FILE] Abrir archivo \"" << filename << "\"\n";

  } else if (subcmd == "find") {
    // TODO: Llamar file_find(filename);
    std::cout << "[FILE] Buscar archivo \"" << filename << "\"\n";

  } else if (subcmd == "size") {
    // TODO: Llamar file_size(filename);
    std::cout << "[FILE] Tamaño de archivo \"" << filename << "\"\n";

  } else {
    std::cerr << "Subcomando .file desconocido\n";
  }
}

void handle_hashd_command(const std::string &str, Directory &d) {
  //HASH DEBUG hashd
  auto parts = split(str, ' ');
  if (parts.size() < 2) {
    std::cerr << "Uso: .hashd <insert|delete|update|search|display> [args...]\n";
    return;
  }

  const std::string &cmd = to_lower(parts[1]);

  try {
    if (cmd == "insert") {
      if (parts.size() < 4) {
        std::cerr << "Uso: .hashd insert <clave:int> <valor:int>\n";
        return;
      }
      int key = std::stoi(parts[2]);
      int value = std::stoi(parts[3]);
      d.insert(key, value, 0);

    } else if (cmd == "delete") {
      if (parts.size() < 4) {
        std::cerr << "Uso: .hashd delete <clave:int> <modo:int>\n";
        return;
      }
      int key = std::stoi(parts[2]);
      int mode = std::stoi(parts[3]);
      d.remove(key, mode);

    } else if (cmd == "update") {
      if (parts.size() < 4) {
        std::cerr << "Uso: .hashd update <clave:int> <nuevo_valor:int>\n";
        return;
      }
      int key = std::stoi(parts[2]);
      int value = std::stoi(parts[3]);
      d.update(key, value);

    } else if (cmd == "search") {
      if (parts.size() < 3) {
        std::cerr << "Uso: .hashd search <clave:int>\n";
        return;
      }
      int key = std::stoi(parts[2]);
      d.search(key);

    } else if (cmd == "display") {
      d.display(false);

    } else if (cmd == "display1") {
      d.display(true);

    } else {
      std::cerr << "Comando .hashd no reconocido: " << cmd << "\n";
    }

  } catch (const std::invalid_argument &e) {
    std::cerr << "Error: argumentos inválidos (esperados enteros)\n";
  } catch (const std::out_of_range &e) {
    std::cerr << "Error: número fuera de rango\n";
  }
}
