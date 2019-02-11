#ifndef ICONMGR_H
#define ICONMGR_H

#include <Qt/XIconManager.h>

class IconMgr {
public:
    DEF_XICON(NO_OP, "://res/no_op.png")
    DEF_XICON(LEFT, "://res/left.png")
    DEF_XICON(RIGHT, "://res/right.png")
    DEF_XICON(ADD, "://res/add.png")
    DEF_XICON(DEL, "://res/del.png")
    DEF_XICO2_EX(LEFT_ADD,LEFT,ADD,Qt::AlignLeft|Qt::AlignBottom)
    DEF_XICO2_EX(RIGHT_ADD,RIGHT,ADD,Qt::AlignRight|Qt::AlignBottom)
    DEF_XICO2_EX(LEFT_DEL,LEFT,DEL,Qt::AlignLeft|Qt::AlignBottom)
    DEF_XICO2_EX(RIGHT_DEL,RIGHT,DEL,Qt::AlignRight|Qt::AlignBottom)
};

#endif // ICONMGR_H
