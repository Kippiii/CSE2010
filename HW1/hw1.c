/*

  Author: Ian Orzel
  Email: iorzel2019@my.fit.edu
  Course: CSE2010
  Section: 23

  Description of this file:
  This program manages a marketplace where a variety of sellers can sell three certain products for a set price.
  Sellers can then increase the inventory of a certain product or have customers buy a certain number of products.
  Then, an analysis of the prices that a certain product is offered by different sellers can be printed.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
  Structure for storing the information on a specific product for a seller
 */
typedef struct {
	double price;	/* Stores the price for one of the product */
	int inventory;	/* Stores the quantity of the product left */
} product_t;

/*
  Structure for storing information about a seller
 */
typedef struct {
	char *name;				/* Stores the pointer to an array of characters representing the name of the seller */
	double shipping_price;		/* Stores the cost of shipping for this seller */
	double min_free_shipping;	/* Stores the minimum cost for an order to qualify for free shipping */
	product_t **products;		/* Stores the pointer to an array of the product_t structs for each of the three products where the indexes come from the product type enum */
} seller_t;

/*
  Structure for storing a Node with a seller_t structure in it for use in a linked list
 */
typedef struct Node {
	seller_t *seller;	/* Stores the pointer to the seller associated with this node */
	struct Node *next;	/* Stores the pointer to the next node in the linked list */
} node_t;

/*
  Enum for storing which command the user has entered
 */
typedef enum Command {
	SetProductPrice, SetShippingCost, IncreaseInventory, CustomerPurchase, DisplaySellerList, UnknownCommand
} command_t;

/*
  Enum for storing which product type a product is
 */
typedef enum Product {
	appleIphone, hdmi2VgaAdapter, USBdrive, UnknownProduct
} product_type_t;

/*
  Function prototypes for functions used throughout the program
 */
void append_node_to_front(node_t **head, node_t *new_node);
command_t convert_str_to_command(char *str);
product_type_t convert_str_to_product(char *str);
node_t *retrieve_seller(node_t *head, char *seller);
seller_t *generate_seller(char *name);
node_t *sort_by_insertion(node_t *cur_head, product_type_t product_type);
double get_price(seller_t *seller, product_type_t product_type);
void free_linked_list(node_t *head, int seller_delete_flag);

/*
  At the top of each function, describe the function, parameter(s), and return value (if any)
 */

int main(int argc, char* argv[]) {
	FILE *in = fopen(argv[1], "r");										/* Stores the file of the input */
	int second_stage_flag = 0;											/* Flag representing when price-setting queries are over */
	node_t *initial_seller_head = NULL;									/* Storing head of linked list for all sellers initially */
	node_t **seller_list_by_product = (node_t **) malloc(3 * sizeof(node_t *));	/* Array for storing each sorted seller list by product */

	/*
     Loops through each line of input until EOF is reached
	 */
	while (!feof(in)) {
		/*
		  Retrieving which command the user is entering
		 */
		char *command_str = (char *) malloc(25 * sizeof(char));	/* Stores the string form of an entered command */
		fscanf(in, "%s", command_str);
		command_t command = convert_str_to_command(command_str);	/* Stores the command_t object of the command */

		if (!second_stage_flag) {
			/*
			  Runs events involving price-setting and then setting up the second stage of the program
			 */
			if (command == SetProductPrice) {
				/*
				  Scans in all of the necessary information given after the command
				 */
				char *product_str = (char *) malloc(20 * sizeof(char));	/* String version of the product type */
				char *seller_str = (char *) malloc(11 * sizeof(char));		/* The seller name being input */
				char *price_str = (char *) malloc(21 * sizeof(char));		/* The string version of the price */
				double price = 0;									/* The double version of the price being set to the product */
				fscanf(in, "%s%s%s", product_str, seller_str, price_str);
				price = strtod(price_str, NULL);

				/*
				  Retrieves the seller from the linked list or generates a seller if one does not yet exist
				 */
				node_t *seller_node = retrieve_seller(initial_seller_head, seller_str);	/* The node object associated with the given seller */
				if (seller_node == NULL) {
					seller_node = (node_t *) malloc(sizeof(node_t));
					seller_node->seller = generate_seller(seller_str);
					seller_node->next = NULL;
					append_node_to_front(&initial_seller_head, seller_node);
				}

				/*
				  Setting the price of the product in the array to the product price being set
				 */
				product_type_t product_type = convert_str_to_product(product_str);	/* The product type object derived from the string */
				product_t *product = seller_node->seller->products[product_type];	/* The product object associated with the given seller and product type */
				product->price = price;

				printf("SetProductPrice %s %s %s\n", product_str, seller_str, price_str);

				/*
				  Freeing unneeded memory
				 */
				free(product_str);
				free(seller_str);
				free(price_str);
			} else if (command == SetShippingCost) {
				/*
				  Scans in all of the necessary information given after the command
				 */
				 char *seller_str = (char *) malloc(11 * sizeof(char));		/* String with the seller name */
				 char *shipping_price_str = (char *) malloc(21 * sizeof(char));	/* String form of the shipping price */
				 double shipping_price = 0;								/* Double value for the shipping price */
				 char *minimum_str = (char *) malloc(21 * sizeof(char));		/* String form of the minimum price for free shipping */
				 double minimum_for_free_shipping = 0;						/* Double value for the minimum price for free shipping */
				 fscanf(in, "%s%s%s", seller_str, shipping_price_str, minimum_str);
				 shipping_price = strtod(shipping_price_str, NULL);
				 minimum_for_free_shipping = strtod(minimum_str, NULL);

				 /*
				   Retrieves the seller from the linked list and generates a new one if it is not there
				  */
				 node_t *seller_node = retrieve_seller(initial_seller_head, seller_str);	/* The node object associated with the given seller */
				 if (seller_node == NULL) {
 					seller_node = (node_t *) malloc(sizeof(node_t));
 					seller_node->seller = generate_seller(seller_str);
					seller_node->next = NULL;
 					append_node_to_front(&initial_seller_head, seller_node);
 				}

				/*
				  Setting the shipping information for the seller
				 */
				seller_node->seller->shipping_price = shipping_price;
				seller_node->seller->min_free_shipping = minimum_for_free_shipping;

				printf("SetShippingCost %s %s %s\n", seller_str, shipping_price_str, minimum_str);

				/*
				  Freeing unneeded memory
				 */
				free(shipping_price_str);
				free(seller_str);
				free(minimum_str);
			} else {
				/*
				  Generate a sorted linked list for each product
				 */
				for (int i = 0; i < 3; i++) {
					seller_list_by_product[i] = sort_by_insertion(initial_seller_head, i);
				}
				second_stage_flag = true;
			}
		}

		if (command == IncreaseInventory) {
			/*
			  Taking the arguments of the command as input
			 */
			char *product_str = (char *) malloc(20 * sizeof(char));			/* Stores the string version of the product type */
			char *seller_str = (char *) malloc(11 * sizeof(char));				/* Stores the name of the seller */
			char *quantity_str = (char *) malloc(21 * sizeof(char));			/* Stores the string version of the quantity being added to the inventory */
			fscanf(in, "%s%s%s", product_str, seller_str, quantity_str);
			int quantity = atoi(quantity_str);								/* Stores the int value of the quantity */
			product_type_t product_type = convert_str_to_product(product_str);	/* Stores the product_type_t object representing the product type */

			/*
			  Finds the seller in the linked list and increases the quantity of the given product
			 */
			seller_t *seller = retrieve_seller(seller_list_by_product[product_type], seller_str)->seller;	/* Variable for storing the seller_t object of the seller */
			seller->products[product_type]->inventory += quantity;

			printf("IncreaseInventory %s %s %s %d\n", product_str, seller_str, quantity_str, seller->products[product_type]->inventory);

			/*
			  Freeing all of the unneeded variables
			*/
			free(product_str);
			free(seller_str);
			free(quantity_str);
		} else if (command == CustomerPurchase) {
			/*
			  Taking the arguments of the command as input
			 */
			char *product_str = (char *) malloc(20 * sizeof(char));			/* Stores the string version of the product type */
			char *seller_str = (char *) malloc(11 * sizeof(char));				/* Stores the seller name */
			char *quantity_str = (char *) malloc(21 * sizeof(char));			/* Stores the string version of the quantity */
			fscanf(in, "%s%s%s", product_str, seller_str, quantity_str);
			int quantity = atoi(quantity_str);								/* Stores the integer version of the quantity */
			product_type_t product_type = convert_str_to_product(product_str);	/* Stores the product_type_t object of the product type */

			/*
			  Finds the seller in the linked list and descreases the inventory by the given amount and logs if it cannot do so
			 */
			seller_t *seller = retrieve_seller(seller_list_by_product[product_type], seller_str)->seller;	/* Variable for storing the pointer to the seller_t object */
			int can_update_inventory_flag = true;												/* Flag for storing whether inventory can be updated */
			(seller->products[product_type]->inventory < quantity) ? (can_update_inventory_flag = false) : (seller->products[product_type]->inventory -= quantity);

			/*
			  Prints the necessary information for the CustomerPurchase command
			 */
			printf("CustomerPurchase %s %s %s ", product_str, seller_str, quantity_str);
			if (can_update_inventory_flag) {
				printf("%d\n", seller->products[product_type]->inventory);
			} else {
				printf("NotEnoughInventoryError\n");
			}

			/*
			  Freeing all of the unneeded variables
			 */
			free(product_str);
			free(seller_str);
			free(quantity_str);
		} else if (command == DisplaySellerList) {
			/*
			  Taking the arguments of the command as input
			 */
			char *product_str = (char *) malloc(20 * sizeof(char));			/* Stores the string version of the product type */
			fscanf(in, "%s", product_str);
			product_type_t product_type = convert_str_to_product(product_str);	/* Strores the product_type_t object of the product type */

			/*
			  Prints the table needed to display the seller list
			 */
			printf("DisplaySellerList %s\n", product_str);
			printf("%10s%14s%14s%11s\n", "seller", "productPrice", "shippingCost", "totalCost");
			for (node_t *tmp = seller_list_by_product[product_type]; tmp != NULL; tmp = tmp->next) {
				if (tmp->seller->products[product_type]->inventory && tmp->seller->products[product_type]->price != -1) {
					printf("%10s%14.2f%14.2f%11.2f\n", tmp->seller->name, tmp->seller->products[product_type]->price, tmp->seller->products[product_type]->price < tmp->seller->min_free_shipping ? tmp->seller->shipping_price : 0, get_price(tmp->seller, product_type));
				}
			}

			/*
			  Freeing the unneeded varaible
			 */
			free(product_str);
		}

		free(command_str);
	}

	/*
	  Freeing all of the linked lists
	 */
	free_linked_list(initial_seller_head, 1);
	for (int i = 0; i < 3; i++) {
		free_linked_list(seller_list_by_product[i], 0);
	}
	free(seller_list_by_product);

	return 0;
}

/*
  Appends a node to be the first element of a linked list
 */
void append_node_to_front(node_t **head, node_t *new_node) {
	/*
	  Sets head as the node after new_node and sets new_node to be the head
	 */
	new_node->next = *head;
	*head = new_node;
}

/*
  Converts the string form of command to the command_t enum
 */
command_t convert_str_to_command(char *str) {
	if (strcmp(str, "SetProductPrice") == 0) {
		return SetProductPrice;
	} else if (strcmp(str, "SetShippingCost") == 0) {
		return SetShippingCost;
	} else if (strcmp(str, "IncreaseInventory") == 0) {
		return IncreaseInventory;
	} else if (strcmp(str, "CustomerPurchase") == 0) {
		return CustomerPurchase;
	} else if (strcmp(str, "DisplaySellerList") == 0) {
		return DisplaySellerList;
	} else {
		return UnknownCommand;
	}
}

/*
  Converts the string form of a product to the product_type_t enum
 */
product_type_t convert_str_to_product(char *str) {
	if (strcmp(str, "appleIphone") == 0) {
		return appleIphone;
	} else if (strcmp(str, "hdmi2VgaAdapter") == 0) {
		return hdmi2VgaAdapter;
	} else if (strcmp(str, "USBdrive") == 0) {
		return USBdrive;
	}
	return UnknownProduct;
}

/*
  Returns the seller node from a list that is given. Returns NULL if no such node exists
 */
node_t *retrieve_seller(node_t *head, char *seller) {
	/*
	  Loops through every node in the list and checks if the seller name is the same as the seller parameter
	 */
	for (node_t *tmp = head; tmp != NULL; tmp = tmp->next) {
		if (strcmp(tmp->seller->name, seller) == 0) {
			return tmp;
		}
	}
	return NULL;
}

/*
  Generates a new seller_t object and returns the pointer
 */
seller_t *generate_seller(char *name) {
	/*
	  Allocates a new seller object and sets its name and other values to defaults
	 */
	seller_t *seller = (seller_t *) malloc(sizeof(seller_t));
	seller->name = (char *) malloc(11 * sizeof(char));
	strcpy(seller->name, name);
	seller->shipping_price = 0;
	seller->min_free_shipping = 0;

	/*
	  Initializes the product array for the seller
	 */
	product_t **products = (product_t **) malloc(3 * sizeof(product_t *));
	for (int i = 0; i < 3; i++) {
		products[i] = (product_t *) malloc(sizeof(product_t));
		products[i]->price = -1;
		products[i]->inventory = 0;
	}
	seller->products = products;

	return seller;
}

/*
  Using the head of a previous linked list and the product number that it is soring by, generates a new sorted linked list
 */
node_t *sort_by_insertion(node_t *cur_head, product_type_t product_type) {
	/*
	  Initializing the new linked list
	 */
	node_t *new_head = (node_t *) malloc(sizeof(node_t));	/* Variable representing the head of the new linked list */
	new_head->seller = cur_head->seller;
	new_head->next = NULL;

	/*
	  Represents the loop of the actual insertion sort into the new linked list
	 */
	for (node_t *cur_node = cur_head->next; cur_node != NULL; cur_node = cur_node->next) {
		seller_t *seller = cur_node->seller;						/* Variable for temporarily storing the current seller */
		double cur_price = get_price(seller, product_type);			/* Variable for storing the price of the product in the new node */
		double head_price = get_price(new_head->seller, product_type);	/* Variable for storing the price of the product for the new head node */

		/*
		  Checks if the new node needs to be placed as the head and does so if true
		 */
		if (cur_price < head_price || (cur_price == head_price && strcmp(seller->name, new_head->seller->name) < 0)) {
			node_t *new_node = (node_t *) malloc(sizeof(node_t));
			new_node->seller = seller;
			new_node->next = new_head;
			new_head = new_node;
			continue;
		}

		/*
		  Loops through the new linked list until it finds place where the new node should be inserted
		 */
		node_t *tmp = new_head;															/* Variable to keep track of the node where the new node needs to be inserted */
		double tmp_price = tmp->next != NULL ? get_price(tmp->next->seller, product_type) : -1;		/* Variable to store the price of the product of the seller in the temp node */
		while (tmp->next != NULL) {
			tmp_price = get_price(tmp->next->seller, product_type);
			if (!(cur_price > tmp_price || (cur_price == tmp_price && strcmp(seller->name, tmp->next->seller->name) > 0))) {
				break;
			}
			tmp = tmp->next;
		}

		/*
		  Inserts the new node after the current position of the temp node
		 */
		node_t *new_node = (node_t *) malloc(sizeof(node_t));
		new_node->seller = seller;
		new_node->next = tmp->next;
		tmp->next = new_node;
	}

	return new_head;
}

/*
  Gets the total price of a given product of a seller
 */
double get_price(seller_t *seller, product_type_t product_type) {
	double price = seller->products[product_type]->price;
	return price + (price < seller->min_free_shipping ? seller->shipping_price : 0);
}

/*
  Frees all of the nodes in the linked list
 */
void free_linked_list(node_t *head, int seller_delete_flag) {
	/*
	  Recursively calls function of the node until reaching the end of the list
	 */
	if (head->next != NULL) {
		free_linked_list(head->next, seller_delete_flag);
	}

	/*
	  Freeing all of the information related to the given node while only removing the seller information once
	 */
	if (seller_delete_flag) {
		for (int i = 0; i < 3; i++) {
			free(head->seller->products[i]);
		}
		free(head->seller->products);
		free(head->seller->name);
		free(head->seller);
		head->seller = NULL;
	}
	free(head);
}
