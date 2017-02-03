typedef struct Node_t Node;
struct Node_t{
 int data;
 struct Node_t * next;
};

typedef struct Bucket_t Bucket;
struct Bucket_t{
  int size;
  struct Node_t * head;
};