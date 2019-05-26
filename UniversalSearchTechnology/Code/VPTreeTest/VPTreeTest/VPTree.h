/**
* @file：VPTree.h
* @description：VP树类的定义，包含创建及查询功能
* @author：杨树
* @data：2019-01-09
*/

#ifndef _VPTREE_HEADER_
#define _VPTREE_HEADER_


#include <vector>
#include <queue>
#include <algorithm>


template<typename T, double(*distance)(T, T)>
class VPTree {
public:
	// 构造函数与析构函数
	VPTree() : _root(0), leafNumber(3){}
	~VPTree() { delete _root; }

	// 设置叶子节点存放数据数目
	void setLeafNumber(int num) {
		leafNumber = num;
	}

	/** 创建VP树
	* @param items：文件中的数据
	*/
	void create(std::vector<T> &items) {
		delete _root;
		_items = items;
		_root = _buildVPTree(0, items.size());
	}

	/** 查找距离最近的一个
	* @param target：目标数据
	* @param results：存放在数据库中查找到的数据
	* @param distances：存放在数据库中的数据到目标数据的距离
	*/
	void query(const T& target, std::vector<T> &results, std::vector<double> &distances) {
		size_t idx;
		double min = std::numeric_limits<double>::max();
		for (size_t i = 0; i<_items.size(); i++) {  // 遍历 _items ，寻找最近的点
			double dist = distance(_items[i], target);
			if (dist < min) {
				min = dist;
				idx = i;
			}
		}
		results.push_back(_items[idx]);
		distances.push_back(min);
	}


	/** k近邻查找
	* @param target：目标数据
	* @param results：存放从数据库中查找到的数据
	* @param distances：存放查到的数据到目标数据的距离
	* @param k：K近邻的k值
	*/
	void knnQuery(const T& target, std::vector<T> &results, std::vector<double> &distances, size_t k) {
		std::priority_queue<DistItem> distQueue;  // 自动对push进去的数据进行按从大到小的排列，DistItem中已重载比较符号“<”

		_kth_dist = std::numeric_limits<double>::max();
		_kquery(_root, target, k, distQueue);

		results.clear(); distances.clear();
		while (!distQueue.empty()) {  // 记录索引和距离
			results.push_back(_items[distQueue.top().index]);
			distances.push_back(distQueue.top().dist);
			distQueue.pop();
		}
		// 将从大到小的顺序颠倒一下，变成距离从小到大的顺序
		std::reverse(results.begin(), results.end());
		std::reverse(distances.begin(), distances.end());
	}

	/** r范围查找
	* @param target：目标数据
	* @param range：查询范围
	* @param results：存放从数据库中查找到的数据
	* @param distances：存放查到的数据到目标数据的距离
	*/
	void rangeQuery(const T& target, double range, std::vector<T> &results, std::vector<double> &distances) {
		std::priority_queue<DistItem> distQueue;  // 自动对push进去的数据进行按从大到小的排列，DistItem中已重载比较符号“<”

		_kth_dist = range;
		_rquery(_root, target, distQueue);

		results.clear(); distances.clear();
		while (!distQueue.empty()) {  // 记录索引和距离
			results.push_back(_items[distQueue.top().index]);
			distances.push_back(distQueue.top().dist);
			distQueue.pop();
		}
		// 将从大到小的顺序颠倒一下，变成距离从小到大的顺序
		std::reverse(results.begin(), results.end());
		std::reverse(distances.begin(), distances.end());
	}

private:
	std::vector<T> _items;  // data array
	double _kth_dist;
	int leafNumber;


	// VP树的叶子节点的定义
	struct VPLeafNode {
		size_t number;  // 叶子结点的数据数目
		std::vector<int> indexs;
	};

	// VP树的内部节点的定义，以及根节点指针 _root 的创建
	struct VPInternalNode {
		int index;  // Vantage Point 的索引
		double radius;  // 划分数据的标准（左子树和右子树的分界）
		struct VPInternalNode* left;
		struct VPInternalNode* right;
		struct VPLeafNode* leftLeaf;
		struct VPLeafNode* rightLeaf;
		VPInternalNode() : index(0), radius(0), left(0), right(0), leftLeaf(0), rightLeaf(0){}
		~VPInternalNode() { delete left; delete right; delete leftLeaf; delete rightLeaf; }
	}*_root;

	struct DistItem {  // 用于查找时存放索引和距离的结构体，和 priority_queue 配合使用
		int index;
		double dist;
		DistItem(int index, double dist) : index(index), dist(dist) {}
		bool operator < (const DistItem& o) const { return dist < o.dist; }
	};
	
	struct DistanceComparator {  // 距离比较器，用于构建VP树的时候作划分，和 nth_element 配合使用
		const T& item;
		DistanceComparator(const T& item) : item(item) {}  // 将有利点（Vantage Point）传进来
		bool operator()(const T& a, const T& b) { return distance(item, a) < distance(item, b); }
	};


	/** 供 create 函数调用，递归构建 VP树
	* @param lower：要划分的数据在数组索引中的下限
	* @param upper：要划分的数据在数组索引中的上限
	* @return：返回内部节点
	*/
	VPInternalNode* _buildVPTree(int lower, int upper) {
		if (upper == lower) return 0;

		VPInternalNode* node = new VPInternalNode();
		node->index = lower;

		// 随机选取一个点作为有利点（Vantage Point）
		int i = (int)((double)rand() / RAND_MAX * (upper - lower - 1)) + lower;
		std::swap(_items[lower], _items[i]);
		int median = (upper + lower) / 2;

		// 划分（partitian）
		std::nth_element(_items.begin() + lower + 1, _items.begin() + median, _items.begin() + upper, DistanceComparator(_items[lower]));

		// 有利点（Vantage Point）
		node->radius = distance(_items[lower], _items[median]);
		node->index = lower;

		if (upper - lower > 2*leafNumber) {
			// 递归构建 VPTree 的内部节点
			node->left = _buildVPTree(lower + 1, median);
			node->right = _buildVPTree(median, upper);
		} else {
			// 构建 VPTree 的叶子结点
			VPLeafNode* Leaf1 = new VPLeafNode();
			Leaf1->number = median - lower - 1;
			for (size_t t = 0; t < Leaf1->number; t++) { Leaf1->indexs.push_back(lower + 1 + t); }
			node->leftLeaf = Leaf1;
			VPLeafNode* Leaf2 = new VPLeafNode();
			Leaf2->number = upper - median;
			for (size_t t = 0; t < Leaf2->number; t++) { Leaf2->indexs.push_back(median + t); }
			node->rightLeaf = Leaf2;
		}
		return node;
	}


	/** 供 knnQuery 函数调用
	* @param node：从 node 节点开始递归向下查询
	* @param target：目标数据
	* @param k：K近邻的k值
	* @param distQueue：优先队列，存放DistItem
	*/
	void _kquery(VPInternalNode* node, const T& target, size_t k, std::priority_queue<DistItem>& distQueue) {
		if (node == NULL) return;

		double dist = distance(_items[node->index], target);
		if (dist <= _kth_dist) {
			if (distQueue.size() == k) distQueue.pop();  // 已经存储了k个值，则删除距离最大的值
			distQueue.push(DistItem(node->index, dist));
			if (distQueue.size() == k) _kth_dist = distQueue.top().dist;  // 更新 _kth_dist 值
		}

		if (dist + _kth_dist >= node->radius) {  // 若 dist(node,target)+r≥M，递归地搜索右子树（球外区域）
			if (node->right != NULL) {
				_kquery(node->right, target, k, distQueue);
			} else {
				for (size_t t = 0; t < node->rightLeaf->number; t++) {
					double dist1 = distance(_items[node->rightLeaf->indexs[t]], target);
					if (dist1 <= _kth_dist) {
						if (distQueue.size() == k) distQueue.pop();
						distQueue.push(DistItem(node->rightLeaf->indexs[t], dist1));
						if (distQueue.size() == k) _kth_dist = distQueue.top().dist;
					}
				}
			}
		}
		if (dist - _kth_dist <= node->radius) {  // 若 dist(node,target)-r≤M，递归地搜索左子树（球内区域）
			if (node->left != NULL) {
				_kquery(node->left, target, k, distQueue);
			} else {
				for (size_t t = 0; t < node->leftLeaf->number; t++) {
					double dist2 = distance(_items[node->leftLeaf->indexs[t]], target);
					if (dist2 <= _kth_dist) {
						if (distQueue.size() == k) distQueue.pop();
						distQueue.push(DistItem(node->leftLeaf->indexs[t], dist2));
						if (distQueue.size() == k) _kth_dist = distQueue.top().dist;
					}
				}
			}
		}
	}


	/** 供 rangeQuery 函数调用
	* @param node：从 node 节点开始递归向下查询
	* @param target：目标数据
	* @param distQueue：优先队列，存放DistItem
	*/
	void _rquery(VPInternalNode* node, const T& target, std::priority_queue<DistItem>& distQueue) {
		if (node == NULL) return;

		double dist = distance(_items[node->index], target);
		if (dist <= _kth_dist) {
			distQueue.push(DistItem(node->index, dist));
		}

		if (dist + _kth_dist >= node->radius) {  // 若 dist(node,target)+r≥M，递归地搜索右子树（球外区域）
			if (node->right != NULL) {
				_rquery(node->right, target, distQueue);
			} else {
				for (size_t t = 0; t < node->rightLeaf->number; t++) {
					double dist1 = distance(_items[node->rightLeaf->indexs[t]], target);
					if (dist1 <= _kth_dist) {
						distQueue.push(DistItem(node->rightLeaf->indexs[t], dist1));
					}
				}
			}
		}
		if (dist - _kth_dist <= node->radius) {  // 若 dist(node,target)-r≤M，递归地搜索左子树（球内区域）
			if (node->left != NULL) {
				_rquery(node->left, target, distQueue);
			} else {
				for (size_t t = 0; t < node->leftLeaf->number; t++) {
					double dist2 = distance(_items[node->leftLeaf->indexs[t]], target);
					if (dist2 <= _kth_dist) {
						distQueue.push(DistItem(node->leftLeaf->indexs[t], dist2));
					}
				}
			}
		}
	}

};


#endif
