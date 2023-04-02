#pragma once
#include "Master.h"

void OutputCategory(struct Category category)
{
    printf("Category's name: %s\n", category.name);
    printf("Category's description: %s\n", category.description);
}

void OutputProduct(struct Product product, struct Category category)
{
    printf("Product's category id %d \n", category.id);
    printf("Category's name and description: %s, %s \n", category.name, category.description);
    printf("Product's name: %s\n", product.name);
    printf("Product's description: %s\n", product.description);
    printf("Product's price: %d\n", product.price);
    printf("Product's weight: %d\n", product.weight);
}
