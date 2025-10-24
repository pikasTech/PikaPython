# 一些基础统计函数：平均值、方差、极值与归一化
# 保持示例简单可读，不依赖外部库。

def mean(nums):
    count = 0
    total = 0
    for x in nums:
        total += x
        count += 1
    return total / count if count else 0

def variance(nums):
    # 使用总体方差公式
    m = mean(nums)
    count = 0
    acc = 0
    for x in nums:
        diff = x - m
        acc += diff * diff
        count += 1
    return acc / count if count else 0

def min_max(nums):
    it = iter(nums)
    try:
        first = next(it)
    except StopIteration:
        return None, None
    mn = mx = first
    for x in it:
        if x < mn:
            mn = x
        if x > mx:
            mx = x
    return mn, mx

def normalize(nums):
    # 线性归一化到 [0,1]; 若所有值相等则返回全 0
    mn, mx = min_max(nums)
    out = []
    if mn is None:
        return out
    span = mx - mn
    if span == 0:
        for _ in nums:
            out.append(0)
        return out
    for x in nums:
        out.append((x - mn) / span)
    return out


if __name__ == '__main__':
    data = [3, 7, 2, 9, 4]
    print('data =', data)
    print('mean =', mean(data))
    print('variance =', variance(data))
    print('min,max =', min_max(data))
    print('normalize =', normalize(data))
