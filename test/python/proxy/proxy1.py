class DataBinding:
    _inner_ = []

    def __init__(self, data):
        _bindings_ = {}
        self._inner_.append(_bindings_)
        self._inner_.append(data)
        # self._inner_[0] = _bindings
        # self._inner_[1] = data

    def __getattr__(self, name):
        data = self._inner_[1]
        return data[name]

    def __setattr__(self, name, value):
        _bindings_ = self._inner_[0]
        if name in _bindings_:
            bindings = _bindings_[name]
            for binding in bindings:
                element = binding['element']
                attr = binding['attr']
                _name = attr.replace("-", "_")
                funcName = "set_%s" % _name
                if hasattr(element, funcName):
                    element.func = getattr(element, funcName)
                    element.func(value)
                else:
                    if hasattr(element, "obj") and element.obj:
                        setattr(element.obj, _name, value)

    def set_binding_value(self, element, attr, key):
        _bindings_ = self._inner_[0]
        if key not in _bindings_:
            _bindings_[key] = []

        _bindings_[key].append({
            "element": element,
            "attr": attr
        })


_data = {
    'a': 10,
    'b': 100
}
data = DataBinding(_data)


class Binding:
    def set_value(self, value):
        print('set value =', value)


binding = Binding()
data.set_binding_value(binding, 'value', 'a')
data.a = 20
