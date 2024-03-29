### Как компилить
clang++ main.cpp -std=c++17
### Условие
Даны α и буква x. Найти максимальное k, такое что в L есть слова, начинающиеся с x^k
### Входные данные
На вход даётся строка из алфавита {a, b, c, ., +, ∗} - регулярное выражение в обратной польской записи и буква x, в конце, через пробел. Пример: ab + c.aba.∗.bac.+.+∗ a
Выход: 2
### Алгоритм решения
По обратной польской записи строим AST регулярного выражения. Определим функцию Func, принимающая вершину поддерева в качестве аргумента и возвращающая два натуральных числа x1, x2 и булевскую переменную x3. x1 - максимальное k, что в поддереве есть слово с префиксом x^k. x2 - максимальное k, что в поддереве есть слово, равное x^k, x3 показывает есть ли в поддереве пустое слово (x3 = does_subtree_have_empty_word). Для некоторых поддеревьев x1 и x2 могут быть неопределены (т.к. по факту их значение бесконечность), но в этом случае алгоритм можно будет тут же завершать, выдавая ответ, подробности позже.

Опишем все случаи запуска Func в зависимости от поддерева.
1. Func запускается от поддерева из одной вершины (ясно, что это вершина с какой-то буквой алфавита), тогда \
x1 = val == x ? 1 : 0 \
x2 = val == x ? 1 : 0, \
x3 = false, \
где val - буква в этой вершине. Возвращая x3 = false, мы значительно используем тот факт, что в регулярном выражении не пристуствует \eps, пустого слова).
2. Func запускается от вершины с +, тогда \
x1 = max(left.x1, right.x1) \
x2 = max(left.x2, right.x2) \
x3 = left.x3 || right.x3, \
где left.x1 это значение x1, которое вернёт Func, запущенная рекурсивно от левого поддерева. Значения остальных определяются аналогично.
Полученные x1 и x2 вычислены верно - тривиально, максимум поддерева достигается для слова, являющимся словом либо из левого поддерева, либо из правого, ну действительно, все слова в языке данного поддерева это либо слова из левого поддерева, либо из правого.
3. Func запускается от вершины с .(конкатенация), тогда \
x1 = max( left.x1, (left.x2 + right.x1) * I(left.x2 > 0 || left.does_subtree_have_empty_word) ) \
x2 = (left.x2 + right.x2) * I(left.x2 > 0 || left.does_subtree_have_empty_word) \
x3 = left.x3 && right.x3 \
Доказательство: слово с наибольшим префиксом в поддереве либо совпадает со словом, соответствующим left.x1, либо было получено, как слово, w1.w2 (конкатенация w1 и w2), где w1 - слово, соответствующее left.x2 и w2 - слово, соответствующее right.x1. Здесь становится ясно зачем мы считаем x3 для всех поддеревьев. Если left.x2 = 0 и left.x3 = false (т.е. пустых слов в левом поддереве нет) w1 не существует, не существует слова, соответствующего значению left.x2, 0 может принимать только пустое слово. Отсюда и появляется домножение на индикатор с таким условием. При выполнении условия w1 и w2 определены - формула рабочая, при невыполнении этого условия ясно, что x1 = 0. Аналогичные рассуждения для x2.
4. Func запускается от звезды Клини. Если единственный ребёнок этого поддерева имеет значение x2 > 0, ясно, что ответ k = inf, завершаем рекурсивный обход, ответ на задачу найден. В противном случае x1 и x2 совпадают с x1 и x2 ребёнка. x3 = true

root.x1 - ответ на задачу
### Ассимптотика
O(#вершин в дереве) = O(длина регулярного выражения)
### Кстати
AST можно не строить и втупую идти по регулярке
