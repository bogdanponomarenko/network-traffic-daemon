#include "binaryTree.h"

int btCompareIP(int ip1[], int ip2[]) {
	int i;
	int res;
	for (i = 0; i < 4; i++) {
		if (*(ip1 + i) > *(ip2 + i)) {
			res = 1;
			break;
		} else if (*(ip1 + i) < *(ip2 + i)) {
		res = -1;
		break;
		} else {
			res = 0;
		}
	}
	return res;
}

void btShow(node **tree, FILE *log) {
	if ((*tree) != NULL) {
		btShow(&(*tree)->ln, log);
		printf("From ip %d.%d.%d.%d  %d packets has been received\n",
		(*tree)->ip[0], (*tree)->ip[1], (*tree)->ip[2], (*tree)->ip[3],
		(*tree)->nPackets);
		btShow(&(*tree)->rn, log);
	}
}

void btDel(node *tree) {
	if ((tree) != NULL) {
		btDel(tree->ln);
		btDel(tree->rn);
		free(tree);
		tree = NULL;
	}
}

void btAddNode(int ip[], node **tree) {
	int cntr;
	int flag;
	flag = 0;
	if (NULL == (*tree)) {
		(*tree) = (node *)malloc(sizeof(node));
		for (cntr = 0; cntr < 4; cntr++)
			(*tree)->ip[cntr] = ip[cntr];
			(*tree)->ln = (*tree)->rn = NULL;
			flag = 1;
			(*tree)->nPackets = 1;
	}
	if (btCompareIP(ip, (*tree)->ip) == -1) {
		if ((*tree)->ln != NULL)
			btAddNode(ip, &((*tree)->ln));
		else {
			(*tree)->ln = (node *)malloc(sizeof(node));
			(*tree)->ln->ln = (*tree)->ln->rn = NULL;
				for (cntr = 0; cntr < 4; cntr++)
					(*tree)->ln->ip[cntr] = ip[cntr];
					(*tree)->ln->nPackets = 1;
		}
	}
	if (btCompareIP(ip, (*tree)->ip) == 1) {
		if ((*tree)->rn != NULL)
			btAddNode(ip, &(*tree)->rn);
		else {
			(*tree)->rn = (node *)malloc(sizeof(node));
			(*tree)->rn->ln = (*tree)->rn->rn = NULL;
			for (cntr = 0; cntr < 4; cntr++)
				(*tree)->rn->ip[cntr] = ip[cntr];
				(*tree)->rn->nPackets = 1;
		}
	}
	if (btCompareIP(ip, (*tree)->ip) == 0 && flag == 0)
	++((*tree)->nPackets);
}

node *btSearch(int ip[], node *tree) {
	if (tree == NULL || btCompareIP(ip, tree->ip) == 0)
		return tree;
	if (btCompareIP(ip, tree->ip) == 1)
		return btSearch(ip, tree->rn);
	if (btCompareIP(ip, tree->ip) == -1)
		return btSearch(ip, tree->ln);
}
