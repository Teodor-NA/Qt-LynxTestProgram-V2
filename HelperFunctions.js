function starWidthSpacing(parent, ratio)
{
    return (parent.width - 2*parent.padding - (parent.children.length - 1)*parent.spacing)*ratio
}

function evenWidthSpacing(parent)
{
    return (parent.width - 2*parent.padding - (parent.children.length - 1)*parent.spacing)/parent.children.length
}

function starHeightSpacing(parent, ratio)
{
    return (parent.height - 2*parent.padding - (parent.children.length - 1)*parent.spacing)*ratio
}

function evenHeightSpacing(parent)
{
    return (parent.height - 2*parent.padding - (parent.children.length - 1)*parent.spacing)/parent.children.length
}

function childrenForcedHeight(object)
{
    var out = 0

    for (var i = 0; i < object.children.length; i++)
    {
        out += object.children[i].height
    }

    return (out + 2*object.padding + object.spacing*(object.children.length - 1))
}
