class CircularQueue:
    def __init__(self, max_size):
        """初始化一个环形队列"""
        self.queue = []
        self.max_size = max_size

    def __getitem__(self, index):
        """通过下标访问队列元素"""
        if len(self.queue) == 0:
            print("Queue is empty")
            return None
        return self.queue[index]

    def __iter__(self):
        """迭代器方法"""
        return iter(self.queue)

    def __contains__(self, item):
        """成员检查方法"""
        return item in self.queue

    def __len__(self):
        """返回队列长度"""
        return len(self.queue)

    def enqueue(self, item):
        """入队操作"""
        if len(self.queue) == self.max_size:
            _ = self.queue.pop(0)
        self.queue.append(item)

    def dequeue(self):
        """出队操作"""
        if len(self.queue) == 0:
            print("Queue is empty")
            return None
        return self.queue.pop(0)

    def remove_by_id(self, index):
        """根据下标移除元素"""
        if len(self.queue) == 0:
            print("Queue is empty")
            return -1
        if index >= len(self.queue) or index < 0:
            print("Index out of range")
            return -1
        _ = self.queue.pop(index)
        return 0

    def remove_by_value(self, value):
        """根据值移除元素"""
        if len(self.queue) == 0:
            print("Queue is empty")
            return -1
        index = -1
        for i in range(len(self.queue)):
            if self.queue[i] == value:
                index = i
                break
        if index == -1:
            # print("Value not found")
            return -1
        _ = self.queue.pop(index)
        return 0

def test():
    queue = CircularQueue(5)
    queue.enqueue(1)
    queue.enqueue(2)
    queue.enqueue(3)
    queue.enqueue(4)
    assert queue[0] == 1
    assert queue[1] == 2
    assert queue.remove_by_id(2) == 0
    assert queue[2] == 4
    assert queue.remove_by_value(1) == 0
    assert queue[0] == 2
    assert queue.dequeue() == 2
    assert queue[0] == 4

    queue = CircularQueue(3)
    queue.enqueue("A")
    queue.enqueue("B")
    queue.enqueue("C")
    assert "A" in queue
    assert "D" not in queue
    assert len(queue) == 3
    assert queue.enqueue("D") == None
    assert "A" not in queue
    assert queue.dequeue() == "B"
    assert queue.remove_by_id(1) == 0
    assert "D" not in queue
    assert queue.remove_by_value("E") == -1
    assert queue.enqueue("1234")
    assert queue.enqueue("test aslkfjawep")
    assert queue.enqueue("alwk")
    # for item in queue:
        # print(item)
    print("PASS")

test()
