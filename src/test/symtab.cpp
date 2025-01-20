#include <gtest/gtest.h>
extern "C"{
  #include "../symtab.h"
}


TEST(SymbolTable, Add) {
  SymTabEntry* entry;
  SymTabEntry* ret; 
  SymTab* table;

  table = symtab_new();
  entry = make_symtab_entry((char*)"int", INT,KEYWORD, NULL);
  symtab_add(table, entry); 

  ret = symtab_get(table,(char*)"int" );

  EXPECT_STREQ(entry->key, ret->key);
  EXPECT_EQ(entry->type, ret->type);
  
}

TEST(SymbolTable, Get){
  SymTab* table;
  SymTabEntry* entry;
  table = symtab_new();

  entry = symtab_get(table, (char *)"entry");
  EXPECT_TRUE(entry == NULL);

}
