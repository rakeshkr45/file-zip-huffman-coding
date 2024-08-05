#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <bitset>

using namespace std;

class BinaryTree {
public:
    char value;
    int frequ;
    BinaryTree* left;
    BinaryTree* right;

    BinaryTree(char value, int frequ) : value(value), frequ(frequ), left(nullptr), right(nullptr) {}

    bool operator<(const BinaryTree& other) const {
        return frequ > other.frequ;  // Note the change in comparison for min-heap
    }

    bool operator==(const BinaryTree& other) const {
        return frequ == other.frequ;
    }
};

class Huffmancode {
private:
    std::string path;
    std::priority_queue<BinaryTree> heap;
    std::unordered_map<char, std::string> code;
    std::unordered_map<std::string, char> reversecode;

    std::unordered_map<char, int> frequency_from_text(const std::string& text) {
        std::unordered_map<char, int> frequ_dict;
        for (char c : text) {
            if (frequ_dict.find(c) == frequ_dict.end()) {
                frequ_dict[c] = 0;
            }
            frequ_dict[c]++;
        }
        return frequ_dict;
    }

    void build_heap(const std::unordered_map<char, int>& frequency_dict) {
        for (const auto& entry : frequency_dict) {
            BinaryTree binary_tree_node(entry.first, entry.second);
            heap.push(binary_tree_node);
        }
    }

    void build_Binary_Tree() {
        while (heap.size() > 1) {
            BinaryTree binary_tree_node_1 = heap.top();
            heap.pop();
            BinaryTree binary_tree_node_2 = heap.top();
            heap.pop();
            int sum_of_freq = binary_tree_node_1.frequ + binary_tree_node_2.frequ;
            BinaryTree newnode('\0', sum_of_freq);
            newnode.left = new BinaryTree(binary_tree_node_1);
            newnode.right = new BinaryTree(binary_tree_node_2);
            heap.push(newnode);
        }
    }

    void build_Tree_Code_Helper(BinaryTree* root, std::string curr_bits) {
        if (root == nullptr) {
            return;
        }
        if (root->value != '\0') {
            code[root->value] = curr_bits;
            reversecode[curr_bits] = root->value;
            return;
        }
        build_Tree_Code_Helper(root->left, curr_bits + '0');
        build_Tree_Code_Helper(root->right, curr_bits + '1');
    }

    void build_Tree_Code() {
        BinaryTree* root = new BinaryTree(heap.top());
        heap.pop();
        build_Tree_Code_Helper(root, "");
        delete root;  // Free memory for the root node
    }

    std::string build_Encoded_Text(const std::string& text) {
        std::string encoded_text = "";
        for (char c : text) {
            encoded_text += code[c];
        }
        return encoded_text;
    }

    std::string build_Padded_Text( std::string& encoded_text) {
        int padding_value = 8 - (encoded_text.length() % 8);
        for (int i = 0; i < padding_value; i++) {
            encoded_text += '0';
        }
        std::string padded_info = std::bitset<8>(padding_value).to_string();
        std::string padded_encoded_text = padded_info + encoded_text;
        return padded_encoded_text;
    }

    std::vector<char> build_Byte_Array(const std::string& padded_text) {
        std::vector<char> array;
        for (size_t i = 0; i < padded_text.length(); i += 8) {
            std::string byte = padded_text.substr(i, 8);
            array.push_back(static_cast<char>(std::stoi(byte, 0, 2)));
        }
        return array;
    }

public:
    Huffmancode(const std::string& p) : path(p) {}

    void compression() {
        cout<<"Compression Starting......"<<endl;
        std::string filename = path.substr(0, path.find_last_of("."));
        std::string output_path = filename + ".bin";
        std::ifstream file(path);
        std::ofstream output(output_path, std::ios::binary);
        std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        text.erase(text.find_last_not_of(" \n\r\t") + 1);
        std::unordered_map<char, int> frequency_dict = frequency_from_text(text);
        build_heap(frequency_dict);
        build_Binary_Tree();
        build_Tree_Code();
        std::string encoded_text = build_Encoded_Text(text);
        std::string padded_text = build_Padded_Text(encoded_text);
        std::vector<char> bytes_array = build_Byte_Array(padded_text);
        output.write(reinterpret_cast<const char*>(&bytes_array[0]), bytes_array.size());
        std::cout << "File Compressed Successfully" << std::endl;
    }
};

int main() {
    std::string path;
    std::cout << "ENTER THE PATH OF YOUR FILE....";
    std::cin >> path;
    Huffmancode h(path);
    h.compression();
    return 0;
}