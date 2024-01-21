#pragma once

char* strtok_r(char* str, const char* delimiters, char** saveptr) 
{
    char* token;

    if (str == NULL)
	{
        str = *saveptr;
	}

    if (*str == '\0') 
	{
        *saveptr = str;
        return NULL;
    }

	str += strspn(str, delimiters);

	if (*str == '\0') 
	{
        *saveptr = str;
        return NULL;
    }

	token = str + strcspn(str, delimiters);
	if (*token == '\0')
    {
      *saveptr = token;
      return str;
    }

	*token = '\0';
	*saveptr = token + 1;
	return str;
}

int strnicmp(const char *str1, const char *str2, size_t n) 
{
    for (size_t i = 0; i < n; i++) 
	{
        if (str1[i] == '\0' || str2[i] == '\0') 
		{
            return 0;
        }
        if (tolower((unsigned char)str1[i]) < tolower((unsigned char)str2[i])) 
		{
            return -1;
        } else if (tolower((unsigned char)str1[i]) > tolower((unsigned char)str2[i])) 
		{
            return 1;
        }
    }
    return 0;
}