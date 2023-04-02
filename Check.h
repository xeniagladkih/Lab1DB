#pragma once

#include "Master.h"
#include "Category.h"
#include "Check.h"

#define INDEXER_SIZE sizeof(struct Indexer)
#define Category_DATA "category.fl"
#define Category_SIZE sizeof(struct Category)
#define PRODUCT_DATA "product.fl"
#define PRODUCT_SIZE sizeof(struct Product)
#define Category_IND "category.ind"

int getCategory(struct Category* category, int id, char* error);
int checkFileExists(FILE* indexTable, FILE* database, char* error)
{
    if (indexTable == NULL || database == NULL) {
        strcpy(error, "DB file does not exit");
        return 0;
    }
    return 1;
}
int checkIndexExists(FILE* indexTable, char* error, int id) {
    fseek(indexTable, 0, SEEK_END);
    long indexTableSize = ftell(indexTable);
    if (indexTableSize == 0 || id * INDEXER_SIZE > indexTableSize) {
        strcpy(error, "No such ID int the table");
        return 0;
    }
    return 1;
}
int checkRecordExists(struct Indexer indexer, char* error) {
    if (indexer.exists == 0 ) {
        strcpy(error, "The record was deleted");
        return 0;
    }
    return 1;
}
int checkIfRecordExist(struct Category category, int productId, char *error ) {
    FILE* productDB = fopen(PRODUCT_DATA, "a+b");
    struct Product product;
    fseek(productDB, category.productFirstAddress, SEEK_SET);
    for (int i = 0; i < category.productsCount; i++) {
        fread(&product, PRODUCT_SIZE, 1, productDB);
        fclose(productDB);
        if (product.Id == productId)
        {
            if(product.exists)
                return 1;
            else
            {
                strcpy(error, "This product was deleted");
                return 0;
            }
        }
        productDB = fopen(PRODUCT_DATA, "r+b");
        fseek(productDB, product.nextAddress, SEEK_SET);
    }
    strcpy(error, "Problem occurred: Either this category don't have this product or the product has been deleted/doesn't exist");
    fclose(productDB);
    return 0;
}
void info() {
    FILE* indexTable = fopen(Category_IND, "rb");
    if (indexTable == NULL) {
        printf("Error: Database files do not exist\n");
        return;
    }
    int categoryCount = 0;
    int productCount = 0;
    fseek(indexTable, 0, SEEK_END);
    int indAmount = ftell(indexTable) / sizeof(struct Indexer);
    struct Category category;
    char error[51];
    for (int i = 1; i <= indAmount; i++)
    {
        if (getCategory(&category, i, error))
        {
            categoryCount++;
            productCount += category.productsCount;
            printf("Category #%d has %d products\n", i, category.productsCount);
        }
    }
    fclose(indexTable);
    printf("Total number of categories: %d\n", categoryCount);
    printf("Total number of products: %d\n", productCount);
}



