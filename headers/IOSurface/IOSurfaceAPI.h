/*
 *  IOSurface.h
 *  IOSurface
 *
 *  Copyright 2006-2008 Apple Computer, Inc. All rights reserved.
 *
 */

#ifndef _IOSURFACE_API_H
#define _IOSURFACE_API_H 1

#include <IOKit/IOKitLib.h>
#include <IOSurface/IOSurfaceBase.h>

__BEGIN_DECLS

typedef struct __IOSurface *IOSurfaceRef;

typedef uint32_t IOSurfaceID;

/* The following list of properties are used with the CFDictionary passed to IOSurfaceCreate(). */

/* kIOSurfaceAllocSize    - CFNumber of the total allocation size of the buffer including all planes.    
				    Defaults to BufferHeight * BytesPerRow if not specified.   Must be specified for
				    dimensionless buffers. */
extern const CFStringRef kIOSurfaceAllocSize;

/* kIOSurfaceWidth  - CFNumber for the width of the IOSurface buffer in pixels.  Required for planar IOSurfaces. */
extern const CFStringRef kIOSurfaceWidth;

/* kIOSurfaceHeight - CFNumber for the height of the IOSurface buffer in pixels. Required for planar IOSurfaces. */
extern const CFStringRef kIOSurfaceHeight;

/* kIOSurfaceBytesPerRow - CFNumber for the bytes per row of the buffer.   If not specified, IOSurface will first calculate
                                   the number full elements required on each row (by rounding up), multiplied by the bytes per element
				   for this buffer.   That value will then be appropriately aligned. */
extern const CFStringRef kIOSurfaceBytesPerRow;

/* Optional properties for non-planar two dimensional images */
 
/* kIOSurfaceBitsPerElement - CFNumber for the total number of bytes in an element.  Default to 1. */
extern const CFStringRef kIOSurfaceBytesPerElement;

/* kIOSurfaceElementWidth   - CFNumber for how many pixels wide each element is.   Defaults to 1. */ 
extern const CFStringRef kIOSurfaceElementWidth;

/* kIOSurfaceElementHeight  - CFNumber for how many pixels high each element is.   Defaults to 1. */ 
extern const CFStringRef kIOSurfaceElementHeight;

/* kIOSurfaceOffset - CFNumber for the starting offset into the buffer.  Defaults to 0. */
extern const CFStringRef kIOSurfaceOffset;

/* Properties for planar surface buffers */

/* kIOSurfacePlaneInfo    - CFArray describing each image plane in the buffer as a CFDictionary.   The CFArray must have at least one entry. */
extern const CFStringRef kIOSurfacePlaneInfo;

/* kIOSurfacePlaneWidth  - CFNumber for the width of this plane in pixels.  Required for image planes. */
extern const CFStringRef kIOSurfacePlaneWidth;

/* kIOSurfacePlaneHeight  - CFNumber for the height of this plane in pixels.  Required for image planes. */
extern const CFStringRef kIOSurfacePlaneHeight;

/* kIOSurfacePlaneBytesPerRow    - CFNumber for the bytes per row of this plane.  If not specified, IOSurface will first calculate
                                   the number full elements required on each row (by rounding up), multiplied by the bytes per element
				   for this plane.   That value will then be appropriately aligned. */
extern const CFStringRef kIOSurfacePlaneBytesPerRow;

/* kIOSurfacePlaneOffset  - CFNumber for the offset into the buffer for this plane.  If not specified then IOSurface
   will lay out each plane sequentially based on the previous plane's allocation size. */
extern const CFStringRef kIOSurfacePlaneOffset;

/* kIOSurfacePlaneSize    - CFNumber for the total data size of this plane.  Defaults to plane height * plane bytes per row if not specified. */
extern const CFStringRef kIOSurfacePlaneSize;

/* Optional properties for planar surface buffers */

/* kIOSurfacePlaneBase    - CFNumber for the base offset into the buffer for this plane. Optional, defaults to the plane offset */
extern const CFStringRef kIOSurfacePlaneBase;

/* kIOSurfacePlaneBytesPerElement    - CFNumber for the bytes per element of this plane.  Optional, default is 1. */
extern const CFStringRef kIOSurfacePlaneBytesPerElement;

/* kIOSurfacePlaneElementWidth    - CFNumber for the element width of this plane.  Optional, default is 1. */
extern const CFStringRef kIOSurfacePlaneElementWidth;

/* kIOSurfacePlaneElementHeight   - CFNumber for the element height of this plane.  Optional, default is 1. */
extern const CFStringRef kIOSurfacePlaneElementHeight;

/* Optional properties global to the entire IOSurface */

/* kIOSurfaceCacheMode		- CFNumber for the CPU cache mode to be used for the allocation.  Default is kIOMapDefaultCache. */
extern const CFStringRef kIOSurfaceCacheMode;

/* kIOSurfaceIsGlobal - CFBoolean     If true, the IOSurface may be looked up by any task in the system by its ID. */
extern const CFStringRef kIOSurfaceIsGlobal;

/* kIOSurfacePixelFormat - CFNumber	A 32-bit unsigned integer that stores the traditional Mac OS X buffer format  */
extern const CFStringRef kIOSurfacePixelFormat;

CFTypeID IOSurfaceGetTypeID(void);

/* Create a brand new IOSurface object */
IOSurfaceRef IOSurfaceCreate(CFDictionaryRef properties);

/* Perform an atomic lookup and retain of a IOSurface by its IOSurfaceID.
   Note: Performing multiple lookups of the same IOSurface will *NOT* return
   the same IOSurfaceRef.   If you need to compare two IOSurface objects
   for equality, you must either do so by comparing their IOSurfaceIDs, or by 
   using CFEqual(). */
IOSurfaceRef IOSurfaceLookup(IOSurfaceID csid);

/* Retrieve the unique IOSurfaceID value for a IOSurface */
IOSurfaceID IOSurfaceGetID(IOSurfaceRef buffer);
	
enum
{
	// If you are not going to modify the data while you hold the lock, you should set this flag to avoid invalidating
	// any existing caches of the buffer contents.  This flag should be passed both to the lock and unlock functions.
	// Non-symmentrical usage of this flag will result in undefined behavior.
	kIOSurfaceLockReadOnly  =		0x00000001,
	
	// If you want to detect/avoid a potentially expensive paging operation (such as readback from a GPU to system memory)
	// when you lock the buffer, you may include this flag.   If locking the buffer requires a readback, the lock will
	// fail with an error return of kIOReturnCannotLock.
	kIOSurfaceLockAvoidSync =		0x00000002
};
			
/* "Lock" or "Unlock" a IOSurface for reading or writing.

    The term "lock" is used loosely in this context, and is simply used along with the
    "unlock" information to put a bound on CPU access to the raw IOSurface data.
    
    If the seed parameter is non-NULL, IOSurfaceLock() will store the buffer's
    internal modification seed value at the time you made the lock call.   You can compare
    this value to a value returned previously to determine of the contents of the buffer
    has been changed since the last lock.
    
    In the case of IOSurfaceUnlock(), the seed value returned will be the internal
    seed value at the time of the unlock.  If you locked the buffer for writing, this value
    will be incremented as the unlock is performed and the new value will be returned.
    
    See the kIOSurfaceLock enums for more information.
    
    Note: Locking and unlocking a IOSurface is not a particularly cheap operation,
    so care should be taken to avoid the calls whenever possible.   The seed values are 
    particularly useful for keeping a cache of the buffer contents.
*/
IOReturn IOSurfaceLock(IOSurfaceRef buffer, uint32_t options, uint32_t *seed);	
IOReturn IOSurfaceUnlock(IOSurfaceRef buffer, uint32_t options, uint32_t *seed);

/* These routines are all fairly self explanatory.  0 is returned if buffer is invalid or NULL */
size_t IOSurfaceGetAllocSize(IOSurfaceRef buffer);

size_t IOSurfaceGetWidth(IOSurfaceRef buffer);
	
size_t IOSurfaceGetHeight(IOSurfaceRef buffer);

size_t IOSurfaceGetBytesPerElement(IOSurfaceRef buffer);

size_t IOSurfaceGetBytesPerRow(IOSurfaceRef buffer);

void *IOSurfaceGetBaseAddress(IOSurfaceRef buffer);

size_t IOSurfaceGetElementWidth(IOSurfaceRef buffer);

size_t IOSurfaceGetElementHeight(IOSurfaceRef buffer);

OSType IOSurfaceGetPixelFormat(IOSurfaceRef buffer);

/* This will return the current seed value of the buffer and is a cheap call to make to see
   if the contents of the buffer have changed since the last lock/unlock. */
uint32_t IOSurfaceGetSeed(IOSurfaceRef buffer);

/* Return the number of planes in this buffer.  May be 0.   Returns 0 for an invalid or NULL buffer pointer. */
size_t IOSurfaceGetPlaneCount(IOSurfaceRef buffer);

/* These routines return information about a particular plane of a IOSurface.   

   If the planeIndex is greater than or equal to the plane count of the IOSurface, zero
   is returned.... with one exception.   If this IOSurface has zero planes and a planeIndex
   of zero is passed in, the routines function just like the non-planar APIs.  This is to allow
   higher level code to treat planar and non-planar buffers is a more uniform fashion. */

size_t IOSurfaceGetWidthOfPlane(IOSurfaceRef buffer, size_t planeIndex);

size_t IOSurfaceGetHeightOfPlane(IOSurfaceRef buffer, size_t planeIndex);

size_t IOSurfaceGetBytesPerElementOfPlane(IOSurfaceRef buffer, size_t planeIndex);

size_t IOSurfaceGetBytesPerRowOfPlane(IOSurfaceRef buffer, size_t planeIndex);

void *IOSurfaceGetBaseAddressOfPlane(IOSurfaceRef buffer, size_t planeIndex);

size_t IOSurfaceGetElementWidthOfPlane(IOSurfaceRef buffer, size_t planeIndex);

size_t IOSurfaceGetElementHeightOfPlane(IOSurfaceRef buffer, size_t planeIndex);

/* These calls let you attach CF property list types to a IOSurface buffer.  These calls are 
   expensive (they essentially must serialize the data into the kernel) and thus should be avoided whenever
   possible.   Note:  These functions can not be used to change the underlying surface properties. */
void IOSurfaceSetValue(IOSurfaceRef buffer, CFStringRef key, CFTypeRef value);

CFTypeRef IOSurfaceCopyValue(IOSurfaceRef buffer, CFStringRef key);

void IOSurfaceRemoveValue(IOSurfaceRef buffer, CFStringRef key);

/* This call lets you get a mach_port_t that holds a reference to the IOSurface. This is useful 
   if you need to atomically or securely pass an IOSurface to another task without making the surface global to
   the entire system.  The returned port must be deallocated with mach_port_deallocate or the equivalent.  
   Note: Any live mach ports created from an IOSurfaceRef implicity increase the IOSurface's global use
   count by one until the port is deleted. */
mach_port_t IOSurfaceCreateMachPort(IOSurfaceRef buffer);

/* This call lets you take a mach_port_t created via IOSurfaceCreatePort() and recreate an IOSurfaceRef from it.
   Note: This call does NOT destroy the port. */
IOSurfaceRef IOSurfaceLookupFromMachPort(mach_port_t port);
	
/* This call lets you get an xpc_object_t that holds a reference to the IOSurface.
   Note: Any live XPC objects created from an IOSurfaceRef implicity increase the IOSurface's global use
   count by one until the object is destroyed. */
//xpc_object_t IOSurfaceCreateXPCObject(IOSurfaceRef aSurface)
//	IOSFC_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_N;

/* This call lets you take an xpc_object_t created via IOSurfaceCreatePort() and recreate an IOSurfaceRef from it. */
//IOSurfaceRef IOSurfaceLookupFromXPCObject(xpc_object_t xobj)
//	IOSFC_AVAILABLE_STARTING(__MAC_10_7, __IPHONE_N;

/* 
   IOSurfaceGetPropertyMaximum() will return the maximum of a given property that is guaranteed to be 
   compatible with all of the current devices (GPUs, etc.) in the system.   The most important ones being:
   
   kIOSurfaceBytesPerRow
   kIOSurfaceWidth
   kIOSurfaceHeight
   kIOSurfacePlaneBytesPerRow
   kIOSurfacePlaneWidth
   kIOSurfacePlaneHeight
   
   For the width and height properties, the maximum values are the largest that are guaranteed to work
   for both reading and writing.   In OpenGL terms this translates into the largest size that will work
   for both textures and render targets.
   
   This function returns 0 for properties that have no predefined limit or where the concept of a limit
   would be considered invalid (such as kIOSurfacePixelFormat).
      
*/   
size_t	IOSurfaceGetPropertyMaximum(CFStringRef property);

/* 
   If a property has a particular alignment requirement, then IOSurfaceGetPropertyAlignment() will return it.  
   If the property has no alignment requirement then 1 will be returned.   The following properties 
   should always be aligned if you choose calculate them yourself:
   
   kIOSurfaceBytesPerRow
   kIOSurfaceOffset
   
   kIOSurfacePlaneBase
   kIOSurfacePlaneOffset
   kIOSurfacePlaneBytesPerRow
   
*/   
size_t	IOSurfaceGetPropertyAlignment(CFStringRef property);

/* This is a convenience function to automatically align property values.  For properties with no alignment
   requirements, the original value will be returned. */
size_t	IOSurfaceAlignProperty(CFStringRef property, size_t value);


/* There are cases where it is useful to know whether or not an IOSurface buffer is considered to be "in use"
   by other users of the same IOSurface.  In particular, CoreVideo and other APIs make use of the IOSurface
   use count facility to know when it is safe to recycle an IOSurface backed CVPixelBuffer object.  This is
   particularly important when IOSurface objects are being shared across process boundaries and the normal
   mechanisms one might use would not be viable.
   
   The IOSurface use count is similar in concept to any othe reference counting scheme.  When the global use
   count of an IOSurface goes to zero, it is no longer considered "in use".   When it is anything other than
   zero, then the IOSurface is still "in use" by someone and therefore anyone attempting to maintain a pool
   of IOSurfaces to be recycled should not reclaim that IOSurface.
   
   Note that IOSurface maintains both a per-process and an internal system wide usage count.   In the current
   implementation, when the per-process usage count goes from zero to one, the system wide usage count is
   incremented by one.   When the per-process usage count drops back to zero (either via explicit decrement
   calls or the process terminates), the global usage count is decremented by one.
   
   IOSurfaceGetUseCount() returns the local per-process usage count for an IOSurface.  This call is only
   provided for logging/debugging purposes and should never be used to determine whether an IOSurface is
   considered to be "in use".   IOSurfaceIsInUse() is the only call that should be used for that purpose. */
   
/* Increment the per-process usage count for an IOSurface */
void IOSurfaceIncrementUseCount(IOSurfaceRef buffer);
	
/* Decrement the per-process usage count for an IOSurface */
void IOSurfaceDecrementUseCount(IOSurfaceRef buffer);

/* Return the per-process usage count for an IOSurface */
int32_t IOSurfaceGetUseCount(IOSurfaceRef buffer);
	
/* Returns true of an IOSurface is in use by any process in the system, otherwise false. */
Boolean IOSurfaceIsInUse(IOSurfaceRef buffer);

__END_DECLS

#endif
