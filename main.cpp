// #include <QApplication>
// #include <QApplication>
// #include <QTableView>
// #include <QVBoxLayout>
// #include <QHeaderView>
// #include <QMouseEvent>
// #include <QStyledItemDelegate>
//
// #include <QDebug>
//
// class HierarchicalItemDelegate : public QStyledItemDelegate {
//     Q_OBJECT
// public:
//     HierarchicalItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
//
//     void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
//         QStyleOptionViewItem opt = option;
//         initStyleOption(&opt, index);
//
//         // Устанавливаем выравнивание текста по левому краю
//         opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
//
//         // Рисуем фон
//         QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
//         style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
//
//         // Рисуем текст слева
//         QRect textRect = opt.rect;
//         textRect.adjust(5, 0, -20, 0); // Отступ слева и справа для текста
//         style->drawItemText(painter, textRect, Qt::AlignLeft | Qt::AlignVCenter, opt.palette, true, opt.text);
//
//         // Рисуем иконку или чекбокс справа
//         if (index.data(Qt::DecorationRole).isValid() && index.column() == 0) {
//             QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
//             if (!icon.isNull()) {
//                 QRect iconRect = opt.rect;
//                 iconRect.setLeft(iconRect.right() - iconRect.width()); // Иконка справа
//                 // iconRect.setWidth(16);
//                 // iconRect.setHeight(16);
//                 icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
//             } else {
//                 qDebug() << "Icon is null for index:" << index.row();
//             }
//         }
//         else if (index.data(Qt::CheckStateRole).isValid() && index.column() == 0) {
//             QStyleOptionButton checkBoxOption;
//             checkBoxOption.state = opt.state;
//             checkBoxOption.state |= (index.data(Qt::CheckStateRole).toInt() == Qt::Checked ? QStyle::State_On : QStyle::State_Off);
//             checkBoxOption.rect = opt.rect;
//             checkBoxOption.rect.setLeft(opt.rect.right() - 16); // Чекбокс справа
//             // checkBoxOption.rect.setWidth(16);
//             // checkBoxOption.rect.setHeight(16);
//             style->drawControl(QStyle::CE_CheckBox, &checkBoxOption, painter, opt.widget);
//         }
//     }
//
//     bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override {
//         if (event->type() == QEvent::MouseButtonRelease && index.column() == 0) {
//             QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
//             QRect checkRect = option.rect;
//             checkRect.setLeft(checkRect.right() - 16);
//             checkRect.setWidth(16);
//             checkRect.setHeight(16);
//
//             if (index.data(Qt::CheckStateRole).isValid() && checkRect.contains(mouseEvent->pos())) {
//                 Qt::CheckState state = (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
//                 return model->setData(index, state, Qt::CheckStateRole);
//             }
//         }
//         return QStyledItemDelegate::editorEvent(event, model, option, index);
//     }
// };
//
// // Кастомная модель для таблицы с иерархическим поведением
// class HierarchicalTableModel : public QAbstractTableModel {
//     Q_OBJECT
//
// public:
//     struct Item {
//         QString name;
//         QString value;
//         bool isHeader;
//         bool isExpanded;
//         Qt::CheckState checkState; // Для чекбоксов в данных
//         QList<Item> children; // Дочерние элементы для заголовка
//     };
//
//     explicit HierarchicalTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
//         QIcon downIcon(":/icons/down_arrow.png");
//         QIcon rightIcon(":/icons/right_arrow.png");
//         qDebug() << "Down arrow icon available:" << !downIcon.isNull();
//         qDebug() << "Right arrow icon available:" << !rightIcon.isNull();
//     }
//
//     // Реализация необходимых методов модели
//     int rowCount(const QModelIndex &parent = QModelIndex()) const override {
//         return items.size();
//     }
//
//     int columnCount(const QModelIndex &parent = QModelIndex()) const override {
//         return 1; // Два столбца: Name и Value
//     }
//
//     QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
//         if (!index.isValid())
//             return QVariant();
//
//         const Item &item = items[index.row()];
//         if (role == Qt::DisplayRole) {
//             if (index.column() == 0)
//                 return item.isHeader ? item.name : QString("    ") + item.name; // Отступ для данных
//             return item.value;
//         }
//         if (role == Qt::DecorationRole && index.column() == 0 && item.isHeader) {
//             // Стрелка для заголовков
//             return item.isExpanded
//                        ? QIcon(QString("review.jpg"))
//                        : QIcon(QString("logo.svg"));
//         }
//         if (role == Qt::CheckStateRole && index.column() == 0 && !item.isHeader) {
//             // Чекбокс для данных
//             return item.checkState;
//         }
//         return QVariant();
//     }
//
//     QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
//         if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//             return section == 0 ? "Name" : "Value";
//         }
//         return QVariant();
//     }
//
//     Qt::ItemFlags flags(const QModelIndex &index) const override {
//         if (!index.isValid())
//             return Qt::NoItemFlags;
//
//         Qt::ItemFlags flags = QAbstractTableModel::flags(index);
//         if (!items[index.row()].isHeader && index.column() == 0) {
//             flags |= Qt::ItemIsUserCheckable; // Включаем чекбоксы для данных
//         }
//         return flags;
//     }
//
//     bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
//         if (!index.isValid())
//             return false;
//
//         Item &item = items[index.row()];
//         if (role == Qt::CheckStateRole && index.column() == 0 && !item.isHeader) {
//             item.checkState = static_cast<Qt::CheckState>(value.toInt());
//             emit dataChanged(index, index, {Qt::CheckStateRole});
//             return true;
//         }
//         return false;
//     }
//
//     // Добавление заголовка
//     void addHeader(const QString &name) {
//         beginInsertRows(QModelIndex(), items.size(), items.size());
//         items.append({name, "Header Value", true, false, Qt::Unchecked, {}});
//         endInsertRows();
//     }
//
//     // Добавление данных под заголовок
//     void addDataUnderHeader(int headerIndex, const QString &name, const QString &value) {
//         if (headerIndex >= 0 && headerIndex < items.size()) {
//             items[headerIndex].children.append({name, value, false, false, Qt::Unchecked, {}});
//         }
//     }
//
//     // Обработка клика по заголовку
//     void toggleHeader(int row) {
//         if (row < 0 || row >= items.size() || !items[row].isHeader)
//             return;
//
//         Item &header = items[row];
//         if (header.isExpanded) {
//             // Сворачиваем: удаляем дочерние строки
//             int childCount = header.children.size();
//             if (childCount > 0) {
//                 beginRemoveRows(QModelIndex(), row + 1, row + childCount);
//                 for (int i = 0; i < childCount; ++i) {
//                     items.removeAt(row + 1);
//                 }
//                 header.isExpanded = false;
//                 endRemoveRows();
//             }
//         } else {
//             // Разворачиваем: вставляем дочерние строки
//             beginInsertRows(QModelIndex(), row + 1, row + header.children.size());
//             for (const auto &child: header.children) {
//                 items.insert(row + 1, child);
//             }
//             header.isExpanded = true;
//             endInsertRows();
//         }
//         emit dataChanged(index(row, 0), index(row, 0), {Qt::DecorationRole});
//     }
//
//     // Получение списка выбранных элементов
//     QStringList getCheckedItems() const {
//         QStringList checked;
//         for (const auto &item: items) {
//             if (!item.isHeader && item.checkState == Qt::Checked) {
//                 checked.append(item.name);
//             }
//         }
//         return checked;
//     }
//
// private:
//     QList<Item> items;
// };
//
// // Кастомный QTableView для обработки кликов
// class HierarchicalTableView : public QTableView {
//     Q_OBJECT
//
// public:
//     HierarchicalTableView(QWidget *parent = nullptr) : QTableView(parent) {
//     }
//
// protected:
//     void mousePressEvent(QMouseEvent *event) override {
//         if (event->button() == Qt::LeftButton) {
//             QModelIndex index = indexAt(event->pos());
//             if (index.isValid()) {
//                 auto *model = dynamic_cast<HierarchicalTableModel *>(this->model());
//                 if (model && index.column() == 0) {
//                     if (model->data(index, Qt::DecorationRole).isValid()) {
//                         model->toggleHeader(index.row());
//                     }
//                 }
//             }
//         }
//         QTableView::mousePressEvent(event);
//     }
// };
//
// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//
//     // Создаем главное окно
//     QWidget window;
//     QVBoxLayout *layout = new QVBoxLayout(&window);
//
//     // Создаем модель
//     HierarchicalTableModel *model = new HierarchicalTableModel(&window);
//
//     // Заполняем модель данными
//     model->addHeader("Category 1");
//     model->addDataUnderHeader(0, "Item 1.1", "Value 1.1");
//     model->addDataUnderHeader(0, "Item 1.2", "Value 1.2");
//
//     model->addHeader("Category 2");
//     model->addDataUnderHeader(1, "Item 2.1", "Value 2.1");
//     model->addDataUnderHeader(1, "Item 2.2", "Value 2.2");
//     model->addDataUnderHeader(1, "Item 2.3", "Value 2.3");
//
//     // Создаем кастомный QTableView
//     HierarchicalTableView* tableView = new HierarchicalTableView(&window);
//     tableView->setModel(model);
//     tableView->setItemDelegate(new HierarchicalItemDelegate(tableView));
//     tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//     tableView->setSelectionMode(QAbstractItemView::SingleSelection);
//     tableView->setShowGrid(true);
//     tableView->setRowHeight(0, 24);
//     tableView->verticalHeader()->setDefaultSectionSize(24);
//
//     // Добавляем таблицу в layout
//     layout->addWidget(tableView);
//
//     // Настраиваем окно
//     window.setWindowTitle("Hierarchical Table");
//     window.resize(400, 300);
//     window.show();
//
//     // Пример получения выбранных элементов (для демонстрации)
//     QObject::connect(tableView, &QTableView::clicked, [&model]() {
//         QStringList checked = model->getCheckedItems();
//         qDebug() << "Checked items:" << checked;
//     });
//
//     return app.exec();
// }
//
// #include <main.moc>

#include <QApplication>
#include <QTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QDebug>

class HierarchicalItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    HierarchicalItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        opt.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;

        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

        QRect textRect = opt.rect;
        int level = index.data(Qt::UserRole).toInt(); // Получаем уровень вложенности
        textRect.adjust(5 + level * 20, 0, -20, 0); // Отступ в зависимости от уровня
        style->drawItemText(painter, textRect, Qt::AlignLeft | Qt::AlignVCenter, opt.palette, true, opt.text);

        if (index.data(Qt::DecorationRole).isValid() && index.column() == 0) {
            QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
            if (!icon.isNull()) {
                QRect iconRect = opt.rect;
                iconRect.setLeft(iconRect.right() - 16);
                iconRect.setWidth(16);
                iconRect.setHeight(16);
                icon.paint(painter, iconRect, Qt::AlignRight | Qt::AlignVCenter);
            }
        } else if (index.data(Qt::CheckStateRole).isValid() && index.column() == 0) {
            QStyleOptionButton checkBoxOption;
            checkBoxOption.state = opt.state;
            checkBoxOption.state |= (index.data(Qt::CheckStateRole).toInt() == Qt::Checked
                                         ? QStyle::State_On
                                         : QStyle::State_Off);
            checkBoxOption.rect = opt.rect;
            checkBoxOption.rect.setLeft(opt.rect.right() - 16);
            checkBoxOption.rect.setWidth(16);
            checkBoxOption.rect.setHeight(16);
            style->drawControl(QStyle::CE_CheckBox, &checkBoxOption, painter, opt.widget);
        }
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override {
        if (event->type() == QEvent::MouseButtonRelease && index.column() == 0) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QRect checkRect = option.rect;
            checkRect.setLeft(checkRect.right() - 16);
            checkRect.setWidth(16);
            checkRect.setHeight(16);

            if (index.data(Qt::CheckStateRole).isValid() && checkRect.contains(mouseEvent->pos())) {
                Qt::CheckState state = (index.data(Qt::CheckStateRole).toInt() == Qt::Checked)
                                           ? Qt::Unchecked
                                           : Qt::Checked;
                return model->setData(index, state, Qt::CheckStateRole);
            }
        }
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
};

class HierarchicalTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    struct Item {
        QString name;
        QString value;
        bool isHeader;
        bool isExpanded;
        Qt::CheckState checkState;
        QList<Item> children;
        int level; // Уровень вложенности
        Item *parent; // Указатель на родителя (для удобства навигации)
    };

    explicit HierarchicalTableModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
        QIcon downIcon(":/icons/down_arrow.png");
        QIcon rightIcon(":/icons/right_arrow.png");
        qDebug() << "Down arrow icon available:" << !downIcon.isNull();
        qDebug() << "Right arrow icon available:" << !rightIcon.isNull();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return flatItems.size();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 1; // Name и Value
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= flatItems.size())
            return QVariant();

        const Item &item = *flatItems[index.row()];
        if (role == Qt::DisplayRole) {
            if (index.column() == 0)
                return item.name;
            return item.value;
        }
        if (role == Qt::DecorationRole && index.column() == 0 && item.isHeader) {
            return item.isExpanded
                       ? QIcon(QString("review.jpg"))
                       : QIcon(QString("logo.svg"));
        }
        if (role == Qt::CheckStateRole && index.column() == 0 && !item.isHeader) {
            return item.checkState;
        }
        if (role == Qt::UserRole) {
            return item.level; // Возвращаем уровень вложенности
        }
        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            return section == 0 ? "Name" : "Value";
        }
        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return Qt::NoItemFlags;

        Qt::ItemFlags flags = QAbstractTableModel::flags(index);
        if (!flatItems[index.row()]->isHeader && index.column() == 0) {
            flags |= Qt::ItemIsUserCheckable;
        }
        return flags;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
        if (!index.isValid() || index.row() >= flatItems.size())
            return false;

        Item &item = *flatItems[index.row()];
        if (role == Qt::CheckStateRole && index.column() == 0 && !item.isHeader) {
            item.checkState = static_cast<Qt::CheckState>(value.toInt());
            emit dataChanged(index, index, {Qt::CheckStateRole});
            return true;
        }
        return false;
    }

    void addHeader(const QString &name, Item *parent = nullptr, int parentIndex = -1) {
        Item *newItem = new Item{
            name, "Header Value", true, false, Qt::Unchecked, {}, parent ? parent->level + 1 : 0, parent
        };
        beginInsertRows(QModelIndex(), flatItems.size(), flatItems.size());
        if (parent) {
            parent->children.append(*newItem);
            if (parent->isExpanded) {
                int insertPos = parentIndex + 1;
                for (int i = parentIndex + 1; i < flatItems.size() && flatItems[i]->level > parent->level; ++i) {
                    insertPos++;
                }
                flatItems.insert(insertPos, newItem);
            }
        } else {
            items.append(*newItem);
            flatItems.append(newItem);
        }
        endInsertRows();
        qDebug() << "Added header:" << name << "at level:" << newItem->level << "parent:" << (
            parent ? parent->name : "none");
    }

    // Добавление данных под заголовок
    void addDataUnderHeader(Item *parent, const QString &name, const QString &value, int parentIndex) {
        if (!parent) return;
        Item *newItem = new Item{name, value, false, false, Qt::Unchecked, {}, parent->level + 1, parent};
        beginInsertRows(QModelIndex(), flatItems.size(), flatItems.size());
        parent->children.append(*newItem);
        if (parent->isExpanded) {
            int insertPos = parentIndex + 1;
            for (int i = parentIndex + 1; i < flatItems.size() && flatItems[i]->level > parent->level; ++i) {
                insertPos++;
            }
            flatItems.insert(insertPos, newItem);
        }
        endInsertRows();
        qDebug() << "Added data:" << name << "under:" << parent->name << "at level:" << newItem->level;
    }


    int addChildrenToFlatList(Item *parent, int parentIndex) {
        int insertPos = parentIndex + 1;
        for (Item &child: parent->children) {
            flatItems.insert(insertPos, &child);
            qDebug() << "Inserted child:" << child.name << "at pos:" << insertPos << "level:" << child.level;
            insertPos++;
            if (child.isHeader && child.isExpanded) {
                insertPos = addChildrenToFlatList(&child, insertPos - 1);
            }
        }
        return insertPos;
    }

    // Обработка клика по заголовку
    void toggleHeader(int row) {
        if (row < 0 || row >= flatItems.size() || !flatItems[row]->isHeader)
            return;

        Item &header = *flatItems[row];
        qDebug() << "Toggling header:" << header.name << "at row:" << row << "isExpanded:" << header.isExpanded;
        if (header.isExpanded) {
            // Сворачиваем: удаляем дочерние строки
            int childCount = 0;
            for (int i = row + 1; i < flatItems.size() && flatItems[i]->level > header.level; ++i) {
                childCount++;
            }
            if (childCount > 0) {
                beginRemoveRows(QModelIndex(), row + 1, row + childCount);
                for (int i = 0; i < childCount; ++i) {
                    flatItems.removeAt(row + 1);
                }
                header.isExpanded = false;
                endRemoveRows();
                qDebug() << "Collapsed:" << header.name << "removed" << childCount << "rows";
            }
        } else {
            // Разворачиваем: вставляем дочерние строки
            beginInsertRows(QModelIndex(), row + 1, row + header.children.size());
            header.isExpanded = true;
            addChildrenToFlatList(&header, row);
            endInsertRows();
            qDebug() << "Expanded:" << header.name << "inserted" << header.children.size() << "rows";
        }
        emit dataChanged(index(row, 0), index(row, 0), {Qt::DecorationRole});
    }

    // Получение списка выбранных элементов
    QStringList getCheckedItems() const {
        QStringList checked;
        for (const auto *item: flatItems) {
            if (!item->isHeader && item->checkState == Qt::Checked) {
                checked.append(item->name);
            }
        }
        return checked;
    }

    // Поиск элемента по строке
    Item *getItemAtRow(int row) {
        return row >= 0 && row < flatItems.size() ? flatItems[row] : nullptr;
    }

private:
    QList<Item> items; // Корневые элементы
    QList<Item *> flatItems; // Плоский список для отображения
};

class HierarchicalTableView : public QTableView {
    Q_OBJECT

public:
    HierarchicalTableView(QWidget *parent = nullptr) : QTableView(parent) {
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            QModelIndex index = indexAt(event->pos());
            if (index.isValid()) {
                auto *model = dynamic_cast<HierarchicalTableModel *>(this->model());
                if (model && index.column() == 0) {
                    if (model->data(index, Qt::DecorationRole).isValid()) {
                        model->toggleHeader(index.row());
                    }
                }
            }
        }
        QTableView::mousePressEvent(event);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);

    HierarchicalTableModel *model = new HierarchicalTableModel(&window);

    // Заполняем модель данными с несколькими уровнями вложенности
    model->addHeader("Category 1");
    model->addDataUnderHeader(model->getItemAtRow(0), "Item 1.1", "Value 1.1", 0);
    model->addHeader("Subcategory 1.2", model->getItemAtRow(0), 0);
    model->addDataUnderHeader(model->getItemAtRow(0)->children[1].parent, "Item 1.2.1", "Value 1.2.1", 0);
    model->addDataUnderHeader(model->getItemAtRow(0)->children[1].parent, "Item 1.2.2", "Value 1.2.2", 0);

    model->addHeader("Category 2");
    model->addDataUnderHeader(model->getItemAtRow(1), "Item 2.1", "Value 2.1", 1);
    model->addDataUnderHeader(model->getItemAtRow(1), "Item 2.2", "Value 2.2", 1);
    model->addHeader("Subcategory 2.3", model->getItemAtRow(1), 1);
    model->addDataUnderHeader(model->getItemAtRow(1)->children[2].parent, "Item 2.3.1", "Value 2.3.1", 1);

    HierarchicalTableView *tableView = new HierarchicalTableView(&window);
    tableView->setModel(model);
    tableView->setItemDelegate(new HierarchicalItemDelegate(tableView));
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setShowGrid(true);
    tableView->setRowHeight(0, 24);
    tableView->verticalHeader()->setDefaultSectionSize(24);

    layout->addWidget(tableView);

    window.setWindowTitle("Hierarchical Table");
    window.resize(400, 300);
    window.show();

    QObject::connect(tableView, &QTableView::clicked, [&model]() {
        QStringList checked = model->getCheckedItems();
        qDebug() << "Checked items:" << checked;
    });

    return app.exec();
}

#include <main.moc>
