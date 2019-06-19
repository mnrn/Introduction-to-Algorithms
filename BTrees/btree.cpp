/**
 * @brief B木のテストプログラム
 * @date  作成日     : 2016/02/05
 * @date  最終更新日 : 2016/02/05
 */



#include "btree.hpp"


int main(void)
{
	BTree<int, int> t(3);
	t.insert(1, 1);
    t.insert(3, 2);
    t.insert(7, 3);
    t.insert(10, 4);
    t.insert(11, 5);
    t.insert(13, 6);
    t.insert(14, 7);
    t.insert(15, 8);
    t.insert(18, 9);
    t.insert(16, 10);
    t.insert(19, 11);
    t.insert(24, 12);
    t.insert(25, 13);
    t.insert(26, 14);
    t.insert(21, 15);
    t.insert(4, 16);
    t.insert(5, 17);
    t.insert(20, 18);
    t.insert(22, 19);
    t.insert(2, 20);
    t.insert(17, 21);
    t.insert(12, 22);
    t.insert(6, 23);
	using namespace std;
 
    cout << "Traversal of the constucted tree is ";
    t.dbg_traverse(t.root);

	t.erase(6);
    cout << "Traversal of tree after removing 6\n";
    t.dbg_traverse(t.root);
    cout << endl;
 
    t.erase(13);
    cout << "Traversal of tree after removing 13\n";
    t.dbg_traverse(t.root);
    cout << endl;
 
    t.erase(7);
    cout << "Traversal of tree after removing 7\n";
    t.dbg_traverse(t.root);
    cout << endl;
 
    t.erase(4);
    cout << "Traversal of tree after removing 4\n";
    t.dbg_traverse(t.root);
    cout << endl;
 
    t.erase(2);
    cout << "Traversal of tree after removing 2\n";
    t.dbg_traverse(t.root);
    cout << endl;
 
    t.erase(16);
	cout << "Traversal of tree after removing 16\n";
    t.dbg_traverse(t.root);
	cout << endl;
	return 0;
}
