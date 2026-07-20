terraform {
  required_version = ">= 1.5"
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
  }
}

provider "aws" {
  region = var.region
}

variable "region" {
  description = "AWS region for IoT Core"
  type        = string
  default     = "eu-west-3"
}

variable "thing_name" {
  description = "IoT Thing name — must match THING_NAME in include/config.h"
  type        = string
  default     = "esp32-telemetry-01"
}

resource "aws_iot_thing" "telemetry_node" {
  name = var.thing_name
}

# Least-privilege policy: the device may only connect as itself and publish
# to its own telemetry topic.
resource "aws_iot_policy" "telemetry_publish" {
  name = "${var.thing_name}-publish"
  policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Effect   = "Allow"
        Action   = ["iot:Connect"]
        Resource = "arn:aws:iot:${var.region}:*:client/${var.thing_name}"
      },
      {
        Effect   = "Allow"
        Action   = ["iot:Publish"]
        Resource = "arn:aws:iot:${var.region}:*:topic/devices/${var.thing_name}/telemetry"
      }
    ]
  })
}

# The device certificate is created out-of-band (the private key is only
# available at creation time and must never enter Terraform state):
#   aws iot create-keys-and-certificate --set-as-active \
#     --certificate-pem-outfile device.pem.crt \
#     --private-key-outfile device.private.key
# Then pass its ARN in:
variable "certificate_arn" {
  description = "ARN of the device certificate created via the AWS CLI"
  type        = string
  default     = null
}

resource "aws_iot_policy_attachment" "device" {
  count  = var.certificate_arn == null ? 0 : 1
  policy = aws_iot_policy.telemetry_publish.name
  target = var.certificate_arn
}

resource "aws_iot_thing_principal_attachment" "device" {
  count     = var.certificate_arn == null ? 0 : 1
  thing     = aws_iot_thing.telemetry_node.name
  principal = var.certificate_arn
}

output "iot_endpoint_hint" {
  value = "Run: aws iot describe-endpoint --endpoint-type iot:Data-ATS --query endpointAddress --output text"
}
