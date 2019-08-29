#include "options.h"

COpt::COpt(QString name): m_name(name)
{

}

COpt::~COpt()
{

}

const QString& COpt::name()
{
    return m_name;
}
