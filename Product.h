#pragma once
#include <malloc.h>
#include "Master.h"
#include "Input.h"
#include "Check.h"
#include "OutputEntities.h"
#include "Slave.h"

#define PRODUCT_DATA "product.fl"
#define PRODUCT_GARBAGE "product_garbage.txt"
#define PRODUCT_SIZE sizeof(struct Product)

int updateCategory(struct Category category, char* error, int id);

struct Product FindLastAddress(FILE *database, struct Category *category, struct Product *previous)
{
    for (int i = 0; i < category->productsCount; i++) {
        fread(previous, PRODUCT_SIZE, 1, database);
        fseek(database, previous->nextAddress, SEEK_SET);
    }
    return (*previous);
}

void NextAddress(FILE* database, struct Category *category, struct Product *product)
{
    fclose(database);
    database = fopen(PRODUCT_DATA, "r+b");
    struct Product previous;
    fseek(database, category->productFirstAddress, SEEK_SET);
    previous = FindLastAddress(database, category, &previous);
    previous.nextAddress = product->selfAddress;
    fwrite(&previous, PRODUCT_SIZE, 1, database);
}
void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Product* record) {
    int * deletedIds = malloc(garbageCount * sizeof(int));
    for (int i = 0; i < garbageCount; i++) {
        fscanf(garbageZone, "%d", deletedIds + i);
    }

    record->selfAddress = deletedIds[0];
    record->nextAddress = deletedIds[0];
    fclose(garbageZone);
    garbageZone = fopen(PRODUCT_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount - 1);
    for (int i = 1; i < garbageCount; i++)
    {
        fprintf(garbageZone, " %d", deletedIds[i]);
    }
    free(deletedIds);
    fclose(garbageZone);
}
int insertProduct(struct Category category, struct Product product, char * error)
{
    FILE* database = fopen(PRODUCT_DATA, "a+b");
    FILE* garbageZone = fopen(PRODUCT_GARBAGE, "a+b");
    int garbageCount = 0;
    fscanf(garbageZone, "%d", &garbageCount);
    product.exists=1;
    struct Product prod;
    fseek(database, 0, SEEK_END);
    if (garbageCount != 0)
    {
        overwriteGarbageAddress(garbageCount, garbageZone, &product);
        fclose(database);
        database = fopen(PRODUCT_DATA, "rb");
        fseek(database, product.selfAddress, SEEK_SET);
        fread(&prod, PRODUCT_SIZE, 1, database);
        product.Id= prod.Id;
        fclose(database);
        database = fopen(PRODUCT_DATA, "r+b");
        fseek(database, product.selfAddress, SEEK_SET);
    }
    else
    {
        if (ftell(database) != 0) {
            fseek(database, -(PRODUCT_SIZE), SEEK_END);
            fread(&prod, PRODUCT_SIZE, 1, database);
            product.Id = prod.Id + 1;
        }
        else
        {
            product.Id = 1;
        }
        int dbSize = ftell(database);
        product.selfAddress = dbSize;
        product.nextAddress = dbSize;
        fseek(database, 0, SEEK_END);
    }
    printf("Your product id is %d \n", product.Id);
    fwrite(&product, PRODUCT_SIZE, 1, database);
    if (!category.productsCount)
    {
        category.productFirstAddress = product.selfAddress;
    }
    else
    {
        NextAddress(database, &category, &product);
    }
    fclose(database);
    int count = category.productsCount + 1;
    category.productsCount=count;
    updateCategory(category, error, category.id);
    return 1;
}
int getProduct(struct Category category, struct Product* product, int productId, char* error) {

    FILE* database = fopen(PRODUCT_DATA, "rb");
    fseek(database, category.productFirstAddress, SEEK_SET);
    fread(product, PRODUCT_SIZE, 1, database);

    for (int i = 0; i < category.productsCount; i++) {
        if (product->Id == productId)
        {
            fclose(database);
            return 1;
        }
        fseek(database, product->nextAddress, SEEK_SET);
        fread(product, PRODUCT_SIZE, 1, database);
    }
    fclose(database);
    return 0;
}
int updateProduct(struct Product product)
{
    FILE* database = fopen(PRODUCT_DATA, "r+b");
    fseek(database, product.selfAddress, SEEK_SET);
    fwrite(&product, PRODUCT_SIZE, 1, database);
    fclose(database);
    return 1;
}
void noteDeletedProduct(int address)
{
    FILE* garbageZone = fopen(PRODUCT_GARBAGE, "rb");
    int garbageCount;
    fscanf(garbageZone, "%d", &garbageCount);
    int* deletedAddresses = malloc(garbageCount * sizeof(long));

    for (int i = 0; i < garbageCount; i++) {
        fscanf(garbageZone, "%d", deletedAddresses + i);
    }
    fclose(garbageZone);
    garbageZone = fopen(PRODUCT_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount + 1);
    for (int i = 0; i < garbageCount; i++) {
        fprintf(garbageZone, " %d", deletedAddresses[i]);
    }

    fprintf(garbageZone, " %d", address);
    free(deletedAddresses);
    fclose(garbageZone);
}
void relinkAddresses(FILE* database, struct Product previous, struct Product product, struct Category* category) {
    if (product.selfAddress == category->productFirstAddress) {
        if (product.selfAddress == product.nextAddress) {
            category->productFirstAddress = -1;
        } else {
            category->productFirstAddress = product.nextAddress;
        }
    } else {
        if (product.selfAddress == product.nextAddress) {
            previous.nextAddress = previous.selfAddress;
        }
        else {
            previous.nextAddress = product.nextAddress;
        }
        fseek(database, previous.selfAddress, SEEK_SET);
        fwrite(&previous, PRODUCT_SIZE, 1, database);
    }
}

void deleteProduct(struct Category category, struct Product product, char* error)
{
    FILE* database = fopen(PRODUCT_DATA, "r+b");
    struct Product previous;
    fseek(database, category.productFirstAddress, SEEK_SET);
    do {
        fread(&previous, PRODUCT_SIZE, 1, database);
        fseek(database, previous.nextAddress, SEEK_SET);
    }
    while (previous.nextAddress != product.selfAddress && product.selfAddress != category.productFirstAddress);
    relinkAddresses(database, previous, product, &category);
    noteDeletedProduct(product.selfAddress);
    product.exists = 0;

    fseek(database, product.selfAddress, SEEK_SET);
    fwrite(&product, PRODUCT_SIZE, 1, database);
    fclose(database);

    category.productsCount--;
    updateCategory(category, error, category.id);
}
void PrintList(struct Category category)
{
    FILE* productDB = fopen(PRODUCT_DATA, "a+b");
    struct Product product;
    fseek(productDB, category.productFirstAddress, SEEK_SET);
    for (int i = 0; i < category.productsCount; i++) {
        fread(&product, PRODUCT_SIZE, 1, productDB);
        if (product.exists)
        {
            OutputProduct(product, category);
            printf("------------------------------------------------------\n");
        }
        fseek(productDB, product.nextAddress, SEEK_SET);
    }
    fclose(productDB);
}