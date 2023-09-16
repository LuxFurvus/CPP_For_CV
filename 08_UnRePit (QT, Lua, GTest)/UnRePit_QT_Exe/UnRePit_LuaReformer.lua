function IsSpace(ch)
    return ch == ' ' or 
    ch == '\n' or 
    ch == '\t' or 
    ch == '\v' or 
    ch == '\r' or 
    ch == '\f'
end

-- Testing the function
--print(IsSpace(' '))    -- true
--print(IsSpace('\n'))   -- true
--print(IsSpace('A'))    -- false
--print(IsSpace('¨'))    -- false
--print(IsSpace('ß'))    -- false
--print(IsSpace('z'))    -- false

function IsBadChar(ch)
    return ch == '•'
end
--print(IsSpace('•'))    -- true
--print(IsSpace('\n'))   -- true
--print(IsSpace('A'))    -- false

function IsSentenceEnd(ch)
    return ch == '.' or 
        ch == '!' or 
        ch == '?' or 
        ch == '…' or 
        ch == ':' or 
        ch == ';'
end

-- Test cases
--print(IsSentenceEnd('.'))   -- Should print: true
--print(IsSentenceEnd('!'))   -- Should print: true
--print(IsSentenceEnd('?'))   -- Should print: true
--print(IsSentenceEnd('…'))   -- Should print: true
--print(IsSentenceEnd(';'))   -- Should print: true

--print(IsSentenceEnd(','))   -- Should print: false
--print(IsSentenceEnd(':'))   -- Should print: false
--print(IsSentenceEnd('A'))   -- Should print: false
--print(IsSentenceEnd(' '))   -- Should print: false
--print(IsSentenceEnd('\t'))  -- Should print: false

function RemoveOddChars(Input)
    local result = ""
    local prevSpace = false
    local prevPunctuation = false

    for i = 1, #Input do
        local char = Input:sub(i, i)
        
        if IsBadChar(char) then
        elseif IsSpace(char) then
            if not prevSpace then
                result = result .. ' '
                prevSpace = true
                prevPunctuation = false  -- Reset punctuation flag on space
            end
        elseif IsSentenceEnd(char) then
            if not prevPunctuation then
                result = result .. char
                prevPunctuation = true
                prevSpace = false  -- Reset space flag on punctuation
            end
        else
            result = result .. char
            prevSpace = false
            prevPunctuation = false
        end
    end

    if IsSpace(result:sub(1, 1)) then  
        return result:sub(2) 
    else
        return result
    end
end

--print(RemoveOddChars("Hello     world!    How   are you?"))
--print(RemoveOddChars("This is a\n\n\ntest    \nstring with various spaces\nand line breaks."))
--print(RemoveOddChars("Thi•s  is  a   t•est   wi•th   spe•cial   ch•ara•cters."))
--print(RemoveOddChars("   There are spaces at the beginning and end.   "))
--print(RemoveOddChars("!!!Thi•s!!!  is  a   t•est   wi•th   repe•ated   exclam•ation   marks!!!"))

function ReformText(Input)
    local TextToReform = RemoveOddChars(Input)
    local Result = ""
    local WasNewLine = false
    local char = ''

    for i = 1, #TextToReform do
        char = TextToReform:sub(i, i)
        if IsSentenceEnd(char) then
            Result = Result .. char
            Result = Result .. '\r'
            Result = Result .. '\n'
            WasNewLine = true
        else
            if (char == ' ' and WasNewLine) then
                WasNewLine = false
            else
                Result = Result .. char
            end
        end
    end

    return Result
end

