#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef FillMemory
#undef FillMemory
#endif
#ifdef ZeroMemory
#undef ZeroMemory
#endif
#ifdef CopyMemory
#undef CopyMemory
#endif
#ifdef MoveMemory
#undef MoveMemory
#endif

__forceinline 
void FillMemory(void *dest,unsigned __int8 c,unsigned __int32 count)
{
	unsigned __int32	size32=count>>2;
	unsigned __int32	fill=(c<<24|c<<16|c<<8|c);
	unsigned __int32	*dest32=(unsigned __int32*)dest;

	switch( (count-(size32<<2)) )
	{
	case 3:	((unsigned __int8*)dest)[count - 3] = c;
	case 2:	((unsigned __int8*)dest)[count - 2] = c;
	case 1:	((unsigned __int8*)dest)[count - 1] = c;
	}

	while( size32-- > 0 )
		*(dest32++) = fill;

}  /* FillMemory */

#define ZeroMemory(dest,count) FillMemory(dest,0,count)

__forceinline 
void CopyMemory(void *dest,const void *src,unsigned __int32 count)
{
	unsigned __int32	size32=count>>2;
	unsigned __int32	*dest32=(unsigned __int32*)dest;
	unsigned __int32	*src32=(unsigned __int32*)src;

	switch( (count-(size32<<2)) )
	{
	case 3:	((unsigned __int8*)dest)[count-1] = 
		((unsigned __int8*)src)[count-1];
	case 2:	((unsigned __int8*)dest)[count-2] = 
		((unsigned __int8*)src)[count-2];
	case 1:	((unsigned __int8*)dest)[count-3] = 
		((unsigned __int8*)src)[count-3];
	}

	while( size32-- > 0 )
		dest32[size32] = src32[size32];

}  /* CopyMemory */

__forceinline 
void MoveMemory(void *dest,const void *src,unsigned __int32 count)
{
	unsigned __int32	size32=count>>2,i;
	unsigned __int32	*dest32=(unsigned __int32*)dest;
	unsigned __int32	*src32=(unsigned __int32*)src;

	if( dest > src )
	{
		switch( (count-(size32<<2)) )
		{
		case 3:	((unsigned __int8*)dest)[count-1] = 
			((unsigned __int8*)src)[count-1];
		case 2:	((unsigned __int8*)dest)[count-2] = 
			((unsigned __int8*)src)[count-2];
		case 1:	((unsigned __int8*)dest)[count-3] = 
			((unsigned __int8*)src)[count-3];
		}

		while( size32-- > 0 )
			dest32[size32] = src32[size32];
	}
	else
	{
		for(i=0;i<size32;i++)
			*(dest32++) = *(src32++);

		switch( (count-(size32<<2)) )
		{
		case 3:	((unsigned __int8*)dest)[count-3] = 
			((unsigned __int8*)src)[count-3];
		case 2:	((unsigned __int8*)dest)[count-2] = 
			((unsigned __int8*)src)[count-2];
		case 1:	((unsigned __int8*)dest)[count-1] = 
			((unsigned __int8*)src)[count-1];
		}
	}

}  /* MoveMemory */

#endif  // _BUFFER_H_

/*
NOTE

The preceding source code uses the __forceinline keyword.
This will achieve greater speed. If small size is desired over speed,
please remove the keyword and use the preceding source as normal functions. 
*/