/// by fanxiushu 2018-08-29

#include "filter.h"

static NTSTATUS commonDispatch(PDEVICE_OBJECT devObj, PIRP irp)
{
	PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);

	switch (irpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:
		break;
	case IRP_MJ_CLEANUP:
		break;
	case IRP_MJ_CLOSE:
		break;

	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		if (irpStack->Parameters.DeviceIoControl.IoControlCode == IOCTL_VIDEO_DDI_FUNC_REGISTER) {
			///////�Կ�������DxgkInitialize�����е��� IOCTL��ȡdxgkrnl.sys��ע��ص�����������hook�˴�����ȡ���Կ������ṩ������DDI����

			irp->IoStatus.Information = 0;
			irp->IoStatus.Status = STATUS_SUCCESS;

			///�����ǵĻص��������ظ��Կ�����.
			if (irp->UserBuffer) {
				///
				irp->IoStatus.Information = sizeof(PDXGKRNL_DPIINITIALIZE);
				*((PDXGKRNL_DPIINITIALIZE*)irp->UserBuffer) = DpiInitialize;
			}

			/////
			IoCompleteRequest(irp, IO_NO_INCREMENT);

			return STATUS_SUCCESS;
			///
		}
		break;
	}
	////
	return call_lower_driver(irp);
}

extern "C" NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT  DriverObject,
	IN PUNICODE_STRING RegistryPath)
{

	NTSTATUS status = STATUS_SUCCESS;

	for (UCHAR i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; ++i) {
		DriverObject->MajorFunction[i] = commonDispatch;
	}

	status = create_wddm_filter_ctrl_device(DriverObject);
	///
	DriverObject->DriverUnload = NULL; ///������ж��

	return status;
}

