#pragma once

#include <Printable.h>
#include <stdbool.h>
#include <stdint.h>

#include "Types.h"

class IComponent : public Printable {
 public:
  /**
   * @brief Call a service.
   * If component has no service it will return immediately.
   * @param service The service number to call.
   */
  virtual void callService(uint8_t service) = 0;

  /**
   * @brief Get an array of all config item values.
   * @param[out] items A list where to write the values.
   * @param length Number of elements the list can max hold.
   * @return Number of items written to the list, 0 if component has no config
   * items.
   */
  virtual uint8_t getConfigValueItems(ValueItemT* items, uint8_t length) const = 0;

  /**
   * @brief Load the config values from persistent storage.
   */
  virtual void loadConfigValues() = 0;

  /**
   * @brief Get an array of all discovery items.
   * @param[out] items A list where to write the items.
   * @param length Number of elements the list can max hold.
   * @return Number of items written to the list.
   */
  virtual uint8_t getDiscoveryItems(DiscoveryEntityItemT* items, uint8_t length) const = 0;

  /**
   * @brief Get the entityId.
   * @return The entity ID.
   */
  virtual uint8_t getEntityId() const = 0;

  /**
   * @brief Get the value item.
   * @param[out] item The value item to write to.
   */
  virtual void getValueItem(ValueItemT* item) const = 0;

  /**
   * @brief Set the value item.
   * @param item The value item to set.
   * @return true if the value was set successfully, false otherwise.
   */
  virtual bool setValueItem(const ValueItemT &item) = 0;

  /**
   * @brief Get the number of entities this component has.
   * @return The number of entities.
   */
  virtual uint8_t getNumEntities() const = 0;

  /**
   * @brief Check if the component's state is due for reporting.
   * @return true if the state is due for reporting, false otherwise.
   */
  virtual bool isReportDue() const = 0;

  /**
   * @brief Print the component's state to a Print object.
   * @param p The Print object to print to.
   * @return The number of characters printed.
   */
  virtual size_t printTo(Print& p) const = 0;

  /**
   * @brief Print the component's state to a Print object.
   * @param p The Print object to print to.
   * @param service The service number to print.
   * @return The number of characters printed.
   */
  virtual size_t printTo(Print& p, uint8_t service) const = 0;

  /**
   * @brief Mark the current state as reported, so that it won't be reported again until it changes.
   */
  virtual void setReported() = 0;

  /**
   * @brief Update the component's state.
   * @return true if the state was updated successfully, false otherwise.
   */
  virtual bool update() = 0;
};
