

import org.usb4java.Context;
import org.usb4java.Device;
import org.usb4java.DeviceDescriptor;
import org.usb4java.DeviceList;
import org.usb4java.LibUsb;
import org.usb4java.LibUsbException;
import org.usb4java.DeviceHandle;
import org.usb4java.DeviceDescriptor;
import org.usb4java.ConfigDescriptor;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import org.usb4java.BufferUtils;
import java.nio.ByteOrder;

/*
 * 
 *  
    public static final int SUCCESS = 0;

    
    public static final int ERROR_IO = -1;

    
    public static final int ERROR_INVALID_PARAM = -2;

    
    public static final int ERROR_ACCESS = -3;

    
    public static final int ERROR_NO_DEVICE = -4;

   
    public static final int ERROR_NOT_FOUND = -5;

    
    public static final int ERROR_BUSY = -6;

    
    public static final int ERROR_TIMEOUT = -7;

   
    public static final int ERROR_OVERFLOW = -8;

    
    public static final int ERROR_PIPE = -9;

    
    public static final int ERROR_INTERRUPTED = -10;

    
    public static final int ERROR_NO_MEM = -11;

    
    public static final int ERROR_NOT_SUPPORTED = -12;

    
    public static final int ERROR_OTHER = -99;

    
    public static final int ERROR_COUNT = 14;

*/

/**
 * Simply lists all available USB devices.
 * 
 * @author Klaus Reimer <k@ailis.de>
 * 
 *  Know Usb End Point
 *  sudo lsusb -v -d vendorid:productid
 * 
 *  //++USB Read Write
 *  https://github.com/usb4java/usb4java-examples/blob/master/src/main/java/org/usb4java/examples/SyncBulkTransfer.java
 * 
 *  ISO/IEC 7816 
 * 
 */

//Bus 001 Device 005: ID 0483:5740 STMicroelectronics STM32F407
//4143:5407
public class ListDevices
{
     
     public static int vendorID  = 0x4143;
     public static int productID = 0x5407 ;
     public static byte bulkEndpointRead= (byte)0x82;
     public static byte bulkEndpointWrite= (byte)0x02;
     
     /**
      * Main method.
      * 
      * @param args
      * Command-line arguments (Ignored)
      */
       public static void main(String[] args)
       {
 	        int interfaceNumber=0;
            // Create the libusb context
			Context context = new Context();
			 //Initialize the libusb context
			int result = LibUsb.init(context);
			if (result < 0)
			{
			    System.out.println("Unable to initialize libusb");
                return;
			    
			}  
   
            Device device = findDevice((short)ListDevices.vendorID,(short)ListDevices.productID); 
            if( null == device )
            {
				System.out.println("Unable to find device");
				return;
			}
            DeviceHandle handle = new DeviceHandle();
			result = LibUsb.open(device, handle);
			if (result != LibUsb.SUCCESS)
            {
                System.out.println("Unable to open USB device");
                return;
            }
			try
			{
								// Use device handle here
								System.out.println("Successfully open USB device");
                                // Dump all configuration descriptors
                                final DeviceDescriptor descriptor = new DeviceDescriptor();
                                result = LibUsb.getDeviceDescriptor(device, descriptor);
								if (result < 0)
								{
									System.out.println("Unable to read device descriptor");
								}
                                
                                //++List Device Descriptor
                                //++dumpConfigurationDescriptors(device, descriptor.bNumConfigurations());

                                
                                /*
                                // Check if kernel driver must be detached
                                boolean value1 = LibUsb.hasCapability(LibUsb.CAP_SUPPORTS_DETACH_KERNEL_DRIVER);

                                int value2 = LibUsb.kernelDriverActive(handle, interfaceNumber);

								boolean detach = true ;

								// Detach the kernel driver
								if (detach)
								{
									result = LibUsb.detachKernelDriver(handle,  interfaceNumber);
									if (result != LibUsb.SUCCESS) 
									{
										System.out.println("Unable to detach kernel driver"); 
										//return;
									}
													  
								}*/


                                result = LibUsb.claimInterface(handle, interfaceNumber);

								if (result != LibUsb.SUCCESS) 
                                { 
                                      System.out.println("Unable to claim interface");
                                      return;
                                }
								try
								{
														//Use interface here
														System.out.println("Successfully claim interface");
														
														byte cmd1[] = new byte[15];
														cmd1[0]  = 0x00;
														cmd1[1]  = (byte)0xA4;
														cmd1[2]  = 0x04;
														cmd1[3]  = 0x00;
														cmd1[4]  = 0x0A;
														cmd1[5]  = (byte)0xA0;
														cmd1[6]  = 0x00;
														cmd1[7]  = 0x00;
														cmd1[8]  = 0x00;
														cmd1[9]  = 0x62;
														cmd1[10] = 0x03;
														cmd1[11] = 0x01;
														cmd1[12] = 0x0C;
														cmd1[13] = 0x06;
														cmd1[14] = 0x01 ;
														//response: 90 00 
														
														byte cmd2[] = new byte[4];
														cmd2[0]  = 0x00;
														cmd2[1]  = (byte)0x00;
														cmd2[2]  = 0x00;
														cmd2[3]  = 0x00;
														//response: 48 65 6C 6C 6F 20 77 6F 72 6C 64 21 90 00
													
														//For String: String text="HelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrldHelloWOrld";

														//For String: ByteBuffer buffer = ByteBuffer.allocateDirect(text.getBytes().length);
														
														
														ByteBuffer buffer = ByteBuffer.allocateDirect(cmd2.length);
				 
														//For String: buffer.put(cmd1.getBytes());
														
														buffer.put(cmd2);
													 
														IntBuffer transfered = IntBuffer.allocate(3);

														byte OUT_ENDPOINT =(byte)ListDevices.bulkEndpointWrite;
								 
														result = LibUsb.bulkTransfer(handle, OUT_ENDPOINT, buffer, transfered, 3000);
								 
														if (result != LibUsb.SUCCESS) 
														{
															System.out.println("Bulk Transfer Write EXCEPTION THROWN");
															//return false;
														}
														System.out.println("LibUSB "+ transfered.get() + " bytes sent to device"); 
														
														//++Read Data
														int size = 14;
														ByteBuffer buffer2 = BufferUtils.allocateByteBuffer(size).order( ByteOrder.LITTLE_ENDIAN);
														IntBuffer transferred = BufferUtils.allocateIntBuffer();
														byte IN_ENDPOINT =(byte)ListDevices.bulkEndpointRead;
														result = LibUsb.bulkTransfer(handle, IN_ENDPOINT, buffer2,transferred, 5000);
														if (result != LibUsb.SUCCESS)
														{
															//throw new LibUsbException("Unable to read data", result);
															System.out.println("Bulk Transfer Read EXCEPTION THROWN: "+result);
															byte[] bytes = new byte[buffer2.remaining()];
															System.out.println("Read Buffer: "+bytes.length);
															for(int counter=0;counter< bytes.length;counter++)
															{
																System.out.println("bytes["+counter+"]: "+ String.format("0x%02x", bytes[counter]) );
															}//for end
														}
														else{
															System.out.println(transferred.get() + " bytes read from device");
														}		
								}
								finally
								{
									result = LibUsb.releaseInterface(handle, interfaceNumber);

									if (result != LibUsb.SUCCESS) 
									{
									  System.out.println("Unable to release interface");
									}
									else
									{
									  System.out.println("Successfully release interface");
									}
									
									LibUsb.close(handle);
									
									System.out.println("Successfully close device handle");
									
								}//finallye end

                               
								/*
                                // Attach the kernel driver again if needed
								if (detach)
								{
									result = LibUsb.attachKernelDriver(handle,  interfaceNumber);
									if (result != LibUsb.SUCCESS){
										System.out.println("Unable to re-attach kernel driver");
									} 
									else{ 
										System.out.println("Successfully re-attach kernel driver");
								    }		
								}*/
								        
			}
			finally
			{
			    //++Deinitialize the libusb context
				LibUsb.exit(context);
				System.out.println("Successfully Deinitialize the libusb context");
			}
			
       }//++public static void main(String[] args) ends

       public static void dumpConfigurationDescriptors(final Device device,final int numConfigurations)
       {
				for (byte i = 0; i < numConfigurations; i += 1)
				{
					final ConfigDescriptor descriptor = new ConfigDescriptor();
					final int result = LibUsb.getConfigDescriptor(device, i, descriptor);
					if (result < 0)
					{
						throw new LibUsbException("Unable to read config descriptor",
							result);
					}
					try
					{
						System.out.println(descriptor.dump().replaceAll("(?m)^",
							"  "));
					}
					finally
					{
						// Ensure that the config descriptor is freed
						LibUsb.freeConfigDescriptor(descriptor);
					}
				}
       
       }//public static void dumpConfigurationDescriptors(final Device device,final int numConfigurations) end

       public void ListDevice()
       {
            // Create the libusb context
			Context context = new Context();
		
			 // Initialize the libusb context
			int result = LibUsb.init(context);
			if (result < 0)
			{
			    throw new LibUsbException("Unable to initialize libusb", result);
			    
			}

			// Read the USB device list
			DeviceList list = new DeviceList();
			result = LibUsb.getDeviceList(context, list);
			if (result < 0)
			{
			    throw new LibUsbException("Unable to get device list", result);
			}

			try
			{
			    // Iterate over all devices and list them
			    for (Device device: list)
			    {
				int address = LibUsb.getDeviceAddress(device);
				int busNumber = LibUsb.getBusNumber(device);
				DeviceDescriptor descriptor = new DeviceDescriptor();
				result = LibUsb.getDeviceDescriptor(device, descriptor);
				if (result < 0)
				{
				    throw new LibUsbException("Unable to read device descriptor", result);
				   
				}
				System.out.format(
				    "Bus %03d, Device %03d: Vendor %04x, Product %04x%n",
				    busNumber, address, descriptor.idVendor(),
				    descriptor.idProduct());
			    }
			}
			finally
			{
			    // Ensure the allocated device list is freed
			    LibUsb.freeDeviceList(list, true);
			}

			// Deinitialize the libusb context
			LibUsb.exit(context);
        
        }//public void ListDevice() end

        public static Device findDevice(short vendorId, short productId)
		{
				// Read the USB device list
				DeviceList list = new DeviceList();
				int result = LibUsb.getDeviceList(null, list);
				if (result < 0) throw new LibUsbException("Unable to get device list", result);

				try
				{
					// Iterate over all devices and scan for the right one
					for (Device device: list)
					{
						DeviceDescriptor descriptor = new DeviceDescriptor();
						result = LibUsb.getDeviceDescriptor(device, descriptor);
						System.out.format(" Vendor %04x, Product %04x%n",descriptor.idVendor(),descriptor.idProduct());
						if (result != LibUsb.SUCCESS) throw new LibUsbException("Unable to read device descriptor", result);
						if (descriptor.idVendor() == vendorId && descriptor.idProduct() == productId) 
						{
							System.out.println("Device Found");
							return device;
						}
					}
				}
				finally
				{
					// Ensure the allocated device list is freed
					LibUsb.freeDeviceList(list, true);
				}

				//Device not found
				return null;
		}//public static Device findDevice(short vendorId, short productId) end
 
}//public class ListDevices end
