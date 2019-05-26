/**
* @file��VPTree.h
* @description��VP����Ķ��壬������������ѯ����
* @author������
* @data��2019-01-09
*/

#ifndef _VPTREE_HEADER_
#define _VPTREE_HEADER_


#include <vector>
#include <queue>
#include <algorithm>


template<typename T, double(*distance)(T, T)>
class VPTree {
public:
	// ���캯������������
	VPTree() : _root(0), leafNumber(3){}
	~VPTree() { delete _root; }

	// ����Ҷ�ӽڵ���������Ŀ
	void setLeafNumber(int num) {
		leafNumber = num;
	}

	/** ����VP��
	* @param items���ļ��е�����
	*/
	void create(std::vector<T> &items) {
		delete _root;
		_items = items;
		_root = _buildVPTree(0, items.size());
	}

	/** ���Ҿ��������һ��
	* @param target��Ŀ������
	* @param results����������ݿ��в��ҵ�������
	* @param distances����������ݿ��е����ݵ�Ŀ�����ݵľ���
	*/
	void query(const T& target, std::vector<T> &results, std::vector<double> &distances) {
		size_t idx;
		double min = std::numeric_limits<double>::max();
		for (size_t i = 0; i<_items.size(); i++) {  // ���� _items ��Ѱ������ĵ�
			double dist = distance(_items[i], target);
			if (dist < min) {
				min = dist;
				idx = i;
			}
		}
		results.push_back(_items[idx]);
		distances.push_back(min);
	}


	/** k���ڲ���
	* @param target��Ŀ������
	* @param results����Ŵ����ݿ��в��ҵ�������
	* @param distances����Ų鵽�����ݵ�Ŀ�����ݵľ���
	* @param k��K���ڵ�kֵ
	*/
	void knnQuery(const T& target, std::vector<T> &results, std::vector<double> &distances, size_t k) {
		std::priority_queue<DistItem> distQueue;  // �Զ���push��ȥ�����ݽ��а��Ӵ�С�����У�DistItem�������رȽϷ��š�<��

		_kth_dist = std::numeric_limits<double>::max();
		_kquery(_root, target, k, distQueue);

		results.clear(); distances.clear();
		while (!distQueue.empty()) {  // ��¼�����;���
			results.push_back(_items[distQueue.top().index]);
			distances.push_back(distQueue.top().dist);
			distQueue.pop();
		}
		// ���Ӵ�С��˳��ߵ�һ�£���ɾ����С�����˳��
		std::reverse(results.begin(), results.end());
		std::reverse(distances.begin(), distances.end());
	}

	/** r��Χ����
	* @param target��Ŀ������
	* @param range����ѯ��Χ
	* @param results����Ŵ����ݿ��в��ҵ�������
	* @param distances����Ų鵽�����ݵ�Ŀ�����ݵľ���
	*/
	void rangeQuery(const T& target, double range, std::vector<T> &results, std::vector<double> &distances) {
		std::priority_queue<DistItem> distQueue;  // �Զ���push��ȥ�����ݽ��а��Ӵ�С�����У�DistItem�������رȽϷ��š�<��

		_kth_dist = range;
		_rquery(_root, target, distQueue);

		results.clear(); distances.clear();
		while (!distQueue.empty()) {  // ��¼�����;���
			results.push_back(_items[distQueue.top().index]);
			distances.push_back(distQueue.top().dist);
			distQueue.pop();
		}
		// ���Ӵ�С��˳��ߵ�һ�£���ɾ����С�����˳��
		std::reverse(results.begin(), results.end());
		std::reverse(distances.begin(), distances.end());
	}

private:
	std::vector<T> _items;  // data array
	double _kth_dist;
	int leafNumber;


	// VP����Ҷ�ӽڵ�Ķ���
	struct VPLeafNode {
		size_t number;  // Ҷ�ӽ���������Ŀ
		std::vector<int> indexs;
	};

	// VP�����ڲ��ڵ�Ķ��壬�Լ����ڵ�ָ�� _root �Ĵ���
	struct VPInternalNode {
		int index;  // Vantage Point ������
		double radius;  // �������ݵı�׼�����������������ķֽ磩
		struct VPInternalNode* left;
		struct VPInternalNode* right;
		struct VPLeafNode* leftLeaf;
		struct VPLeafNode* rightLeaf;
		VPInternalNode() : index(0), radius(0), left(0), right(0), leftLeaf(0), rightLeaf(0){}
		~VPInternalNode() { delete left; delete right; delete leftLeaf; delete rightLeaf; }
	}*_root;

	struct DistItem {  // ���ڲ���ʱ��������;���Ľṹ�壬�� priority_queue ���ʹ��
		int index;
		double dist;
		DistItem(int index, double dist) : index(index), dist(dist) {}
		bool operator < (const DistItem& o) const { return dist < o.dist; }
	};
	
	struct DistanceComparator {  // ����Ƚ��������ڹ���VP����ʱ�������֣��� nth_element ���ʹ��
		const T& item;
		DistanceComparator(const T& item) : item(item) {}  // �������㣨Vantage Point��������
		bool operator()(const T& a, const T& b) { return distance(item, a) < distance(item, b); }
	};


	/** �� create �������ã��ݹ鹹�� VP��
	* @param lower��Ҫ���ֵ����������������е�����
	* @param upper��Ҫ���ֵ����������������е�����
	* @return�������ڲ��ڵ�
	*/
	VPInternalNode* _buildVPTree(int lower, int upper) {
		if (upper == lower) return 0;

		VPInternalNode* node = new VPInternalNode();
		node->index = lower;

		// ���ѡȡһ������Ϊ�����㣨Vantage Point��
		int i = (int)((double)rand() / RAND_MAX * (upper - lower - 1)) + lower;
		std::swap(_items[lower], _items[i]);
		int median = (upper + lower) / 2;

		// ���֣�partitian��
		std::nth_element(_items.begin() + lower + 1, _items.begin() + median, _items.begin() + upper, DistanceComparator(_items[lower]));

		// �����㣨Vantage Point��
		node->radius = distance(_items[lower], _items[median]);
		node->index = lower;

		if (upper - lower > 2*leafNumber) {
			// �ݹ鹹�� VPTree ���ڲ��ڵ�
			node->left = _buildVPTree(lower + 1, median);
			node->right = _buildVPTree(median, upper);
		} else {
			// ���� VPTree ��Ҷ�ӽ��
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


	/** �� knnQuery ��������
	* @param node���� node �ڵ㿪ʼ�ݹ����²�ѯ
	* @param target��Ŀ������
	* @param k��K���ڵ�kֵ
	* @param distQueue�����ȶ��У����DistItem
	*/
	void _kquery(VPInternalNode* node, const T& target, size_t k, std::priority_queue<DistItem>& distQueue) {
		if (node == NULL) return;

		double dist = distance(_items[node->index], target);
		if (dist <= _kth_dist) {
			if (distQueue.size() == k) distQueue.pop();  // �Ѿ��洢��k��ֵ����ɾ����������ֵ
			distQueue.push(DistItem(node->index, dist));
			if (distQueue.size() == k) _kth_dist = distQueue.top().dist;  // ���� _kth_dist ֵ
		}

		if (dist + _kth_dist >= node->radius) {  // �� dist(node,target)+r��M���ݹ����������������������
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
		if (dist - _kth_dist <= node->radius) {  // �� dist(node,target)-r��M���ݹ����������������������
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


	/** �� rangeQuery ��������
	* @param node���� node �ڵ㿪ʼ�ݹ����²�ѯ
	* @param target��Ŀ������
	* @param distQueue�����ȶ��У����DistItem
	*/
	void _rquery(VPInternalNode* node, const T& target, std::priority_queue<DistItem>& distQueue) {
		if (node == NULL) return;

		double dist = distance(_items[node->index], target);
		if (dist <= _kth_dist) {
			distQueue.push(DistItem(node->index, dist));
		}

		if (dist + _kth_dist >= node->radius) {  // �� dist(node,target)+r��M���ݹ����������������������
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
		if (dist - _kth_dist <= node->radius) {  // �� dist(node,target)-r��M���ݹ����������������������
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
