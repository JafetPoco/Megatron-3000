#ifndef FREEBLOCKMANAGER_HPP
#define FREEBLOCKMANAGER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

using BlockID = ssize_t;

class FreeBlockManager {
private:
  std::fstream file;
  std::string filename;
  std::size_t totalBlocks;
  std::vector<char> bitmap;

  /**
   * @brief Escribe el mapa de bits en el disco
   * @author Berly Dueñas
   */
  void persist(); 

public:
  /**
   * @brief Abre el bit map del disco (si es que existe), de lo contrario crea uno nuevo
   * @param diskname Nombre del disco
   * @param numBlocks Numero de bloques del disco
   * @author Berly Dueñas
   */
  FreeBlockManager(std::string diskname, std::size_t numBlocks);
  /**
   * @brief Busca en el bit map el primer bloque vacio y devuelve su Id
   * @return BlockID Id de un bloque
   * @author Berly Dueñas
   */
  BlockID allocateBlock();
  /**
   * @brief Cambia el valor a 0 en el bit map, liberando el bloque para su uso en el futuro
   * @param id ID del bloque
   * @author Berly Dueñas
   */
  void freeBlock(BlockID id);
  /**
   * @brief Me indica si el bloque con el id indicado está ocupado o vacio
   * @param id Id del bloque
   * @return bool true si está libre, false si está ocupado
   * @author Berly Dueñas
   */
  bool isBlockFree(BlockID id) const;
  /**
   * @brief Cuenta cuantos bloques vacios existen
   * @return size_t Numero de bloques vacios
   * @author Berly Dueñas
   */
  size_t freeBlockCount() const;
  /**
   * @brief (No documentado)
   * @return bool
   * @author Berly Dueñas
   */
  bool deallocateBlock();
};

#endif
