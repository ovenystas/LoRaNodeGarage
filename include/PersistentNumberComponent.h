#pragma once

#include "Component.h"
#include "PersistentNumber.h"

template <class T>
class PersistentNumberComponent : public IComponent {
 public:
  PersistentNumberComponent() = delete;

  PersistentNumberComponent(PersistentNumber<T>& persistentNumber)
      : mPersistentNumber{persistentNumber} {}

  void callService(uint8_t service) final { (void)service; }

  bool getConfigValue(ValueItemT& item, uint8_t index) const final {
    if (index >= sNumConfigItems) {
      return false;
    }

    // PersistentNumberComponent has no config items

    return true;
  }

  void loadConfigValues() final {};

  bool getDiscoveryEntity(DiscoveryEntityT& item) const final {
    mPersistentNumber.getDiscoveryEntity(item);
    return true;
  }

  uint8_t getEntityId() const final { return mPersistentNumber.getEntityId(); }

  uint8_t getNumEntities() const final { return sNumItems; };

  uint8_t getNumConfigItems() const final { return sNumConfigItems; }

  T getValue() const { return mPersistentNumber.getValue(); }

  void getValueItem(ValueItemT& item) const final {
    return mPersistentNumber.getValueItem(item);
  }

  bool setValueItem(const ValueItemT& item) final {
    mPersistentNumber.setValueItem(item);
    return true;
  }

  bool isReportDue() const final { return mPersistentNumber.isReportDue(); }

  size_t printTo(Print& p) const final { return mPersistentNumber.printTo(p); };

  size_t printTo(Print& p, uint8_t service) const final {
    (void)p;
    (void)service;
    return 0;
  };

  void setReported() final { mPersistentNumber.setReported(); }

  void setValue(T value) { mPersistentNumber.setValue(value); }

  bool update() final { return false; };

  /**
   * @brief Get the number of entities this component provides
   * @return Number of entities (used for entity ID assignment in main.cpp)
   */
  static constexpr uint8_t getEntityCount() { return sNumItems; }

 private:
  static constexpr uint8_t sNumConfigItems = 3;
  static constexpr uint8_t sNumItems = 1 + sNumConfigItems;
  PersistentNumber<T>& mPersistentNumber;
};
