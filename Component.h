#pragma once

#include <Stream.h>
#include <stdbool.h>
#include <stdint.h>

#include "Types.h"

class IComponent {
 public:
  /**
   * @brief Call a service.
   * If component has no service it will return immediately.
   * @param service The service number to call.
   */
  virtual void callService(uint8_t service) = 0;

  /**
   * @brief Get an array of all config item values.
   * @param[in, out] items A list where to write the values.
   * @param length Number of elements the list can max hold.
   * @return Number of items written to the list, 0 if component has no config
   * items.
   */
  virtual uint8_t getConfigItemValues(ConfigItemValueT* items,
                                      uint8_t length) const = 0;

  virtual void getDiscoveryItem(DiscoveryItemT* item) const = 0;

  /**
   * @brief Get the entityId.
   * @return The entity ID.
   */
  virtual uint8_t getEntityId() const = 0;

  virtual void getValueItem(ValueItemT* item) const = 0;

  virtual bool isReportDue() const = 0;

  virtual size_t print(Stream& stream) const = 0;

  virtual size_t print(Stream& stream, uint8_t service) const = 0;

  virtual bool setConfigItemValues(const ConfigItemValueT* items,
                                   uint8_t length) = 0;

  virtual void setReported() = 0;

  virtual bool update() = 0;
};
