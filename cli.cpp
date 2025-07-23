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
    std::cout << "Comandos soportados:\n";
    std::cout << "  SELECT columnas FROM tabla [WHERE condición];\n";
    std::cout << "  DELETE FROM tabla [WHERE condición];\n";
    std::cout << "  INSERT INTO tabla (col1, col2, ...) VALUES (val1, val2, ...);\n";
    std::cout << "  addcsv archivo.csv tabla\n";
    std::cout << "  exit  - salir del programa\n";
    std::cout << "Notas:\n";
    std::cout << "  - Las flechas ↑ ↓ navegan el historial\n";
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

    } else if (entity == "sector") {
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
