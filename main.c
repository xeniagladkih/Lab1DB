#include <stdio.h>
#include "Master.h"
#include "Input.h"
#include "OutputEntities.h"
#include "Slave.h"
#include "Category.h"
#include "Product.h"

#define INDEXER_SIZE sizeof(struct Indexer)
int main()
{
    struct Category category;
    struct Product product;
    while (1)
    {
        int choice;
        int id;
        char error[20];

        printf("------------------------------------------------------\n");
        printf("Acceptable Options:\n0 Insert category\n1 Get category\n2 Update category\n3 Delete category\n4 Output list of categories\n5 Insert product\n6 Get product\n7 Update product\n8 Delete product\n9 Output list of products\n10 General Info\n11 Exit\n");
        printf("------------------------------------------------------\n");
        printf("Your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 0:
                inputCategory(&category);
                insertCategory(category);
                break;
            case 1:
                printf("Enter category ID: \n");
                scanf("%d", &id);
                if(getCategory(&category, id, error) != 0)
                {
                    OutputCategory(category);
                }
                else
                {
                    printf("Error: %s\n", error);
                }
                break;
            case 2:
                printf("Enter category ID: \n");
                scanf("%d", &id);
                getCategory(&category, id, error);
                inputCategory(&category);
                if(updateCategory(category, error, id) == 0)
                    printf("%s", error);
                else
                    printf("Updated successfully!\n");
                break;
            case 3:
                printf("Enter ID: \n");
                scanf("%d", &id);
                if(deleteCategory(id, error) != 0)
                    printf("Deleted successfully!\n");
                else
                    printf("Error: %s\n", error);
                break;
            case 4:
                PrintListOfCategory(error);
                break;
            case 5:
                printf("Enter category ID: \n");
                scanf("%d", &id);
                if(getCategory(&category, id, error) != 0)
                {
                    InputProduct(&product);
                    insertProduct(category, product, error);
                    printf("Inserted successfully! \n");
                }
                else
                {
                    printf("Error %s\n", error);
                }
                break;
            case 6:
                printf("Enter category ID: \n");
                scanf("%d", &id);
                if(getCategory(&category, id, error))
                {
                    printf("Enter product's ID: \n");
                    scanf("%d", &id);
                    if(checkIfRecordExist(category, id, error))
                    {
                        getProduct(category, &product, id, error);
                        OutputProduct(product, category);
                    }
                    else
                    {
                        printf("Error %s\n", error);
                    }
                }
                else
                {
                    printf("Error %s\n", error);
                }
                break;
            case 7:
                printf("Enter category ID: \n");
                scanf("%d", &id);
                if(getCategory(&category, id, error))
                {
                    printf("Enter product's ID:\n ");
                    scanf("%d", &id);
                    if(checkIfRecordExist(category, id, error))
                    {
                        getProduct(category, &product, id, error);
                        InputProduct(&product);
                        updateProduct(product);
                        printf("Updated successfully!\n ");
                    }
                    else
                    {
                        printf("Error %s\n", error);
                    }
                }
                else
                {
                    printf("Error %s \n", error);
                }

                break;
            case 8:
                printf("Enter category ID: \n");
                scanf("%d", &id);
                if(getCategory(&category, id, error))
                {
                    printf("Enter product's ID:\n");
                    scanf("%d", &id);
                    if(checkIfRecordExist(category, id, error))
                    {
                        getProduct(category, &product, id, error);
                        deleteProduct(category, product, error);
                        printf("Deleted successfully! \n");
                    }
                    else
                    {
                        printf("Error %s \n ", error);
                    }
                }
                else
                {
                    printf("Error %s \n ", error);
                }
                break;
            case 9:
                printf("Enter category ID: \n");
                scanf("%d", &id);
                if(getCategory(&category, id, error))
                {
                    if(category.productsCount != 0)
                    {
                        PrintList(category);
                    }
                    else
                    {
                        printf("This category has no products\n");
                    }
                }
                else
                {
                    printf("Error %s \n ", error);
                }
                break;
            case 10:
                info();
                break;
            case 11:
                return 0;
            default:
                printf("No command chosen. \n");
        }
        printf("\n");
    }
}