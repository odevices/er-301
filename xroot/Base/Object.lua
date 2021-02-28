local Object = {
  __class__ = "Object",
  __instance__ = nil
}

function Object:__tostring()
  if self.__instanceName__ then
    if self.__instanceKey__ then
      return string.format("%s(%s,%s)", self.__className__,
                           self.__instanceName__, self.__instanceKey__)
    else
      return string.format("%s(%s)", self.__className__, self.__instanceName__)
    end
  else
    if self.__instanceKey__ then
      return string.format("%s(%s)", self.__className__, self.__instanceKey__)
    else
      return self.__className__
    end
  end
end

function Object:setClassName(name)
  self.__className__ = name
end

function Object:getClassName()
  return self.__className__
end

function Object:setInstanceName(name)
  self.__instanceName__ = name
end

function Object:getInstanceName()
  return self.__instanceName__
end

function Object:getInstanceKey()
  return self.__instanceKey__
end

function Object:setInstanceKey(key)
  self.__instanceKey__ = key
end

return Object
