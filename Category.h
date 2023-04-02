#pragma once
#include <malloc.h>
#include "Master.h"
#include "Input.h"
#include "Check.h"
#include "OutputEntities.h"
#include "Slave.h"
#include "Product.h"

#define Category_GARBAGE "category_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
int PrintListOfCategory(char *error);
void overwriteGarbageCategoryElement(int garbageCount, FILE* garbageZone, struct Category* category)
{
    int* deleteIdsFile = malloc(garbageCount * sizeof(int));
    for (int i = 0; i < garbageCount; i++)
    {
        fscanf(garbageZone, "%d", deleteIdsFile + i);
    }
    fclose(garbageZone);
    category->id = deleteIdsFile[0];
    fopen(Category_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount - 1);
    for (int i = 1; i < garbageCount; i++)
    {
        fprintf(garbageZone, " %d", deleteIdsFile[i]);
    }
    fclose(garbageZone);
    free(deleteIdsFile);
}
int insertCategory(struct Category category){
    FILE* garbageZone = fopen(Category_GARBAGE, "a+b");
    FILE* indexTable = fopen(Category_IND, "a+b");
    FILE* database = fopen(Category_DATA, "a+b");
    int garbageCount = 0;
    fscanf(garbageZone, "%d", &garbageCount);
    struct Indexer indexer;
    if (garbageCount !=0 )
    {
        overwriteGarbageCategoryElement(garbageCount, garbageZone, &category);
        fclose(database);
        fclose(indexTable);
        indexTable = fopen(Category_IND, "r+b");
        database = fopen(Category_DATA, "r+b");
        fseek(indexTable, (category.id - 1) * INDEXER_SIZE, SEEK_SET);
        fread(&indexer, INDEXER_SIZE, 1, indexTable);
        fseek(database, indexer.address, SEEK_SET);
    }
    else {
        fseek(indexTable, 0, SEEK_END);
        if (ftell(indexTable) != 0) {
            fseek(indexTable, -(INDEXER_SIZE), SEEK_END);
            fread(&indexer, INDEXER_SIZE, 1, indexTable);
            category.id = indexer.id + 1;
        } else {
            category.id = 1;
        }
    }
    category.productFirstAddress = -1;
    category.productsCount = 0;
    fwrite(&category, Category_SIZE, 1, database);
    indexer.id = category.id;
    indexer.exists = 1;
    indexer.address = (category.id - 1) * Category_SIZE;
    printf("Your category id is %d\n", category.id);
    fseek(indexTable, (category.id - 1) * INDEXER_SIZE, SEEK_SET);
    fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
    fclose(database);
    fclose(indexTable);
    return 1;
}
int getCategory(struct Category* category, int id, char* error)
{
    FILE* indexTable = fopen(Category_IND, "rb");
    FILE* database = fopen(Category_DATA, "rb");
    if(checkFileExists(indexTable, database, error) == 0)
        return 0;
    struct Indexer indexer;
    if(checkIndexExists(indexTable, error, id) ==0)
        return 0;
    fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
    fread(&indexer, INDEXER_SIZE, 1, indexTable);
    if (checkRecordExists(indexer, error)==0)
        return 0;
    fseek(database, indexer.address, SEEK_SET);
    fread(category, Category_SIZE, 1, database);
    fclose(indexTable);
    fclose(database);
    return 1;
}

int updateCategory(struct Category category, char* error, int id) {
    FILE* indexTable = fopen(Category_IND, "r+b");
    FILE* database = fopen(Category_DATA, "r+b");
    struct Indexer indexer;
    if (checkFileExists(indexTable, database, error)==0) {
        return 0;
    }
    if (checkIndexExists(indexTable, error, id)==0) {
        return 0;
    }
    fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
    fread(&indexer, INDEXER_SIZE, 1, indexTable);
    if(checkRecordExists(indexer, error)==0)
        return 0;
    category.id = id;
    fseek(database, indexer.address, SEEK_SET);
    fwrite(&category, Category_SIZE, 1, database);
    fclose(indexTable);
    fclose(database);
    return 1;
}
void listOfDeletedCategory(int id)
{
    FILE* garbage = fopen(Category_GARBAGE, "rb");
    if (garbage == NULL)
    {
        printf("Error occurred during opening file \n");
        return;
    }
    int garbageCount;
    fscanf(garbage, "%d", &garbageCount);
    int* deleteIds = malloc(garbageCount * sizeof(int));

    for (int i = 0; i < garbageCount; i++)
    {
        fscanf(garbage, "%d", deleteIds + i);
    }
    fclose(garbage);
    garbage = fopen(Category_GARBAGE, "wb");
    fprintf(garbage, "%d", garbageCount + 1);
    for (int i = 0; i < garbageCount; i++) {
        fprintf(garbage, " %d", deleteIds[i]);
    }
    fprintf(garbage, " %d", id);
    free(deleteIds);
    fclose(garbage);
}
int deleteCategory(int id, char* error) {
    FILE* indexTable = fopen(Category_IND, "r+b");
    struct Indexer indexer;
    if (indexTable == NULL) {
        strcpy(error, "Database files are not created yet");
        return 0;
    }
    if (checkIndexExists(indexTable, error, id)==0) {
        return 0;
    }
    fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
    fread(&indexer, INDEXER_SIZE, 1, indexTable);
    if(checkRecordExists(indexer, error)==0)
        return 0;
    struct Category category;
    getCategory(&category, id, error);
    indexer.exists = 0;
    fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
    fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
    fclose(indexTable);
    listOfDeletedCategory(id);
    if (category.productsCount != 0) {
        FILE* productDB = fopen(PRODUCT_DATA, "r+b");
        struct Product product;
        fseek(productDB, category.productFirstAddress, SEEK_SET);
        for (int i = 0; i < category.productsCount; i++)
        {
            fread(&product, PRODUCT_SIZE, 1, productDB);
            fclose(productDB);
            deleteProduct(category, product, error);
            productDB = fopen(PRODUCT_DATA, "r+b");
            fseek(productDB, product.nextAddress, SEEK_SET);
        }
        fclose(productDB);
    }
    return 1;
}
int PrintListOfCategory(char *error)
{
    FILE* database = fopen(Category_DATA, "rb");
    FILE* indexTable = fopen(Category_IND, "rb");
    struct Category category;
    struct Indexer indexer;
    if (checkFileExists(indexTable, database, error)==0)
    {
        return 0;
    }
    fseek(indexTable, -INDEXER_SIZE, SEEK_END);
    fread(&indexer, INDEXER_SIZE, 1, indexTable);
    rewind(indexTable);
    int endIndex =0;
    getCategory(&category, indexer.id, error);
    endIndex = indexer.id;
    int id = 0;
    int count =0;
    while(id < endIndex)
    {
        fseek(indexTable, id *INDEXER_SIZE, SEEK_SET);
        fread(&indexer, INDEXER_SIZE, 1, indexTable);
        if(indexer.exists)
        {
            printf("------------------------------------------------------\n");
            getCategory(&category, indexer.id, error);
            printf("Id %d\n", category.id);
            OutputCategory(category);
            count++;
        }
        id++;
    }
    if(count == 0)
    {
        printf("No categories\n");
    }
    return 1;
}

